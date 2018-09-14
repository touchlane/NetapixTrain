//
//  run.c
//  netapix
//
//  Created by Pavel Kondrashkov on 6/5/18.
//  Copyright © 2018 Touchlane LLC. All rights reserved.
//

#include "run.h"
#include <stdlib.h>
#include <string.h>
#include "constants.h"
#include "connected.h"
#include "loss.h"
#include "train.h"
#include "convolutional.h"

run_root *make_run_root(layer_config *configs, int layers_count, float *input_data) {
    run_root *root = malloc(sizeof(*root));
    root->input_data = input_data;
    root->weights = malloc(layers_count * sizeof(*root->weights));
    int i;
    for (i = 0; i < layers_count; ++i) {
        root->weights[i] = make_run_weights(configs[i]);
    }
    root->network = make_run_network(configs, layers_count, root->weights, input_data);
    return root;
}

int free_run_root(run_root *root) {
    for (int i = 0; i < root->network.count; ++i) {
        free_run_weights(root->weights[i]);
    }
    free(root->weights);
    free_run_network(root->network);
    free(root);
    return 0;
}

run_weights make_run_weights(layer_config config) {
    run_weights weights;
    weights.type = WEIGHT_RUN_TYPE;
    switch (config.type) {
        case CONNECTED:
            weights.count = (config.input_length + 1) * config.output_length;
            weights.values = calloc(weights.count, sizeof(*weights.values));
            break;
        case CONVOLUTIONAL:
            weights.count = config.width * config.height * config.input_depth * config.channels + config.channels;
            weights.values = calloc(weights.count, sizeof(*weights.values));
            break;
        default:
            printf(FATAL_ERROR_MAKE_WEIGHTS_FAIL_MSG);
            exit(1);
    }
    return weights;
}

int free_run_weights(run_weights weights) {
    free(weights.values);
    return 0;
}

run_network make_run_network(layer_config *configs, int layers_count, run_weights *weights, float *input_data) {
    run_network net;
    net.count = layers_count;
    net.layers = malloc((layers_count) * sizeof(*net.layers));
    float *prev_output = input_data;
    for (int i = 0; i < layers_count; i++) {
        switch (configs[i].type) {
            case CONVOLUTIONAL:
                net.layers[i] = make_convolutional_layer(configs[i], weights[i].values, NULL, NULL, prev_output, NULL);
                prev_output = ((convolutional_layer*)net.layers[i])->output;
                break;
            case CONNECTED:
                net.layers[i] = make_connected_layer(configs[i], weights[i].values, NULL, NULL, prev_output, NULL);
                prev_output = ((connected_layer*)net.layers[i])->output;
                break;
            default:
                printf(FATAL_ERROR_MAKE_NETWORK_FAIL_MSG);
                exit(1);
        }
    }
    return net;
}

int free_run_network(run_network net) {
    int i;
    for (i = 0; i < net.count; ++i) {
        int is_first_layer = i == 0;
        free_layer(net.layers[i], is_first_layer);
    }
    free(net.layers);
    return 0;
}

int run(char *npi_path, char *weights_path, char *output_path) {
    int input_length;
    float *input_data = NULL;
    if (read_npi(npi_path, &input_data, &input_length)) {
        return 0;
    }
    
    int layers_count;
    layer_config *configs;
    if (read_layer_configs_from_npw(weights_path, &configs, &layers_count)) {
        return 0;
    }
    
    run_root *root = make_run_root(configs, layers_count, input_data);
    read_npw(weights_path, root->weights, layers_count);
    
    for (int i = 0; i < layers_count; i++) {
        forward(root->network.layers[i]);
    }
    
    void *last_layer = root->network.layers[layers_count - 1];
    write_layer_output(output_path, last_layer);
    
    free_run_root(root);
    return 0;
}

int forward(void *layer) {
    layer_type *type = (layer_type *)layer;
    switch (*type) {
        case CONNECTED:
            connected_forward((connected_layer *)layer);
            break;
        case LOSS:
            loss_forward((loss_layer *)layer);
            break;
        case CONVOLUTIONAL:
            convolutional_forward((convolutional_layer *)layer);
            break;
        default:
            printf(FATAL_ERROR_ACCESS_NDF_LAYER_FAIL_MSG);
            exit(1);
            break;
    }
    return 0;
}

int read_npi(char *path, float **input, int *input_size) {
    FILE *file = fopen(path, "rb");
    if (!file) {
        printf(ERROR_OPEN_FILE_MSG, path);
        return ERROR_OPEN;
    }
    
    fseek(file, 0, SEEK_END);
    *input_size = (int)ftell(file) / sizeof(**input);
    fseek(file, 0, SEEK_SET);
    *input = malloc(*input_size * sizeof(**input));
    size_t success = fread(*input, sizeof(**input), *input_size, file);
    if (success != *input_size) {
        printf(FATAL_ERROR_NPI_VALUES_FORMAT_FAIL_MSG, *input_size, (int)success, path);
        fclose(file);
        return ERROR_FORMAT;
    }
    
    fclose(file);
    return 0;
}

int write_npo(char *path, float *output, int output_length) {
    /// BINARY MODE
    FILE *file = fopen(path, "wb");
    if (!file) {
        printf(ERROR_OPEN_FILE_MSG, path);
        return ERROR_OPEN;
    }
    fwrite(output, sizeof(*output), output_length, file);
    fclose(file);

    /// TEXT MODE
//    FILE *file = fopen(path, "w");
//    if (!file) {
//        printf(ERROR_OPEN_FILE_MSG, path);
//        return ERROR_OPEN;
//    }
//    int i;
//    for (i = 0; i < output_length; ++i) {
//        fprintf(file, i == 0 ? "%f" : ", %f", output[i]);
//    }
//    fclose(file);
    return 0;
}


int write_layer_output(char *path, void *layer) {
    layer_type *type = (layer_type *)layer;
    switch (*type) {
        case CONNECTED: {
            connected_layer *connected = (connected_layer *)layer;
            write_npo(path, connected->output, connected->output_length);
        }
            break;
        case CONVOLUTIONAL: {
            convolutional_layer *convolutional = (convolutional_layer *)layer;
            write_npo(path, convolutional->output, convolutional->output_length);
        }
            break;
        case LOSS:
            printf(FATAL_ERROR_WRITE_LOSS_LAYER_NPO_FAIL_MSG);
            exit(1);
            break;
        default:
            printf(FATAL_ERROR_ACCESS_NDF_LAYER_FAIL_MSG);
            exit(1);
            break;
    }
    return 0;
}

int free_layer(void *layer, int is_first_layer) {
    layer_type *type = (layer_type *)layer;
    switch (*type) {
        case CONNECTED:
            free_connected_layer((connected_layer *)layer, is_first_layer);
            break;
        case LOSS:
            free_loss_layer((loss_layer *)layer);
            break;
        case CONVOLUTIONAL:
            free_convolutional_layer((convolutional_layer *)layer, is_first_layer);
            break;
        case NDEF_LAYER:
            printf(FATAL_ERROR_ACCESS_NDF_LAYER_FAIL_MSG);
            exit(1);
            break;
    }
    return 0;
}

int read_layer_configs_from_npw(char *path, layer_config **configs, int *layers_count) {
    FILE *file = fopen(path, "rb");
    if (!file) {
        printf(ERROR_OPEN_FILE_MSG, path);
        return ERROR_OPEN;
    }
    size_t success = 0;
    success = fread(layers_count, sizeof(*layers_count), 1, file);
    if (success != 1) {
        printf(FATAL_ERROR_NPW_FORMAT_FAIL_MSG, path);
        fclose(file);
        return ERROR_FORMAT;
    }
    *configs = malloc( (*layers_count) * sizeof(**configs));
    int i;
    for (i = 0; i < *layers_count; ++i) {
        success = fread(&(*configs)[i], sizeof(layer_config), 1, file);
        if (success != 1) {
            printf(FATAL_ERROR_NPW_CONFIG_FORMAT_FAIL_MSG, path);
            fclose(file);
            return ERROR_FORMAT;
        }
        int values_count = 0;
        layer_config config = (*configs)[i];
        layer_type type = (*configs)[i].type;
        switch (type) {
            case CONNECTED:
                values_count = ((*configs)[i].input_length + 1) * (*configs)[i].output_length;
                break;
            case CONVOLUTIONAL:
                values_count = config.width * config.height * config.input_depth * config.channels + config.channels;
                break;
            default:
                printf(FATAL_ERROR_MAKE_WEIGHTS_FAIL_MSG);
                break;
        }
        fseek(file, values_count * sizeof(float), SEEK_CUR);
    }
    fclose(file);
    return 0;
}
