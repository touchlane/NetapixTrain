//
//  train.c
//  netapix
//
//  Created by Evgeny Dedovets on 4/28/18.
//  Copyright © 2018 Touchlane LLC. All rights reserved.
//

#include "train.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <dirent.h>
#include <string.h>

#include "constants.h"
#include "math.h"
#include "connected.h"
#include "loss.h"
#include "run.h"
#include "convolutional.h"
#include "validation.h"

sem_t *sem;

void *compute_network(void *n) {
    sem_wait(sem);
    train_network *net = n;
    int item_position = *net->iteration_number * net->batch + net->position_in_batch;
    read_npt(net->dataset->training_set[*net->train_set_number]->items[item_position],
             input_from_layer(net->layers[0]),
             ((loss_layer *)net->layers[net->count - 1])->target,
             net->input_length, net->target_length);
    for (int i = 0; i < net->count; i++) {
        forward(net->layers[i]);
    }
    backward(net->layers[net->count - 1]);
    for (int i = net->count - 2; i > 0; i--) {
        backward(net->layers[i]);
        calc_corrections(net->layers[i]);
    }
    calc_corrections(net->layers[0]);
    sem_post(sem);
    return NULL;
}

float reg_add(regularization_type type, float lambda, float multiplier) {
    float res = 0;
    switch (type) {
        case L1:
            res = lambda;
            break;
        case L2:
            res = lambda * multiplier;
            break;
        case NDEF_REGULARIZATION:
            res = 0;
            break;
    }
    return res;
}

void *update_weights(void *w) {
    sem_wait(sem);
    train_weights *weights = w;
    int i, j;
    float delta, gradient;
    switch (weights->params->learning) {
        case GRADIENT_DESCENT:
             for (i = 0; i < weights->count; i++) {
                 gradient = 0;
                 for (j = 0; j < weights->params->batch; j++) {
                     gradient = gradient + weights->corrections[j][i];
                     weights->corrections[j][i] = 0;
                 }
                 delta = weights->params->eta * (gradient + reg_add(weights->params->regularization, weights->params->lambda, weights->values[i])) + weights->params->momentum * weights->prev_delta[i];
                 weights->prev_delta[i] = delta;
                 weights->values[i] = weights->values[i] - delta;
             }
            break;
        case NDEF_LEARN:
            printf(FATAL_ERROR_NDF_LEARNING_TYPE_FAIL_MSG);
            exit(1);
            break;
    }
    sem_post(sem);
    return NULL;
}

int backward(void *layer) {
    layer_type *type = (layer_type *)layer;
    switch (*type) {
        case CONNECTED:
            connected_backward((connected_layer *)layer);
            break;
        case LOSS:
            loss_backward((loss_layer *)layer);
            break;
        case CONVOLUTIONAL:
            convolutional_backward((convolutional_layer *)layer);
            break;
        default:
            printf(FATAL_ERROR_ACCESS_NDF_LAYER_FAIL_MSG);
            exit(1);
            break;
    }
    return 0;
}

int calc_corrections(void *layer) {
    layer_type *type = (layer_type *)layer;
    switch (*type) {
        case CONNECTED:
            calc_connected_corrections((connected_layer *)layer);
            break;
        case CONVOLUTIONAL:
            calc_сonvolutional_сorrections((convolutional_layer *)layer);
            break;
        default:
            printf(FATAL_ERROR_ACCESS_NDF_LAYER_FAIL_MSG);
            exit(1);
            break;
    }
    return 0;
}

float *input_from_layer(void *layer) {
    float *res = NULL;
    layer_type *type = (layer_type *)layer;
    switch (*type) {
        case CONNECTED:
            res = ((connected_layer *)layer)->input;
            break;
        case LOSS:
            res = ((loss_layer *)layer)->input;
            break;
        case CONVOLUTIONAL:
            res = ((convolutional_layer *)layer)->input;
            break;
        case NDEF_LAYER:
            printf(FATAL_ERROR_ACCESS_NDF_LAYER_FAIL_MSG);
            exit(1);
            break;
    }
    return res;
}

int train(char *npx_path, char *train_path, char *weights_path, char *output_path) {
    npx_config *npx = read_npx(npx_path);
    if (validate_npx(npx) < 0) {
        free_npx_config(npx);
        return NPX_FAIL;
    }
    
    input_data *raw_input_data = read_train_data(train_path);
    int validation_items_count = validation_size(raw_input_data->count, npx->settings->validation);
    int is_data_valid = validate_data(raw_input_data,
                                      npx->settings->input_length,
                                      npx->settings->target_length,
                                      npx->settings->batch,
                                      validation_items_count);
    if (is_data_valid < 0) {
        free_npx_config(npx);
        free_input_data(raw_input_data);
        return NPX_FAIL;
    }
    
    shuffle(raw_input_data);
    data_set *dataset = prepare_data_set(raw_input_data, validation_items_count);
    train_root *root = make_train_root(npx, dataset);
    if (weights_path) {
        read_npw(weights_path, root->weights, npx->size - 1);
    } else {
        init_weights(root->weights, npx);
    }
    validation_root *validation = make_validation_root(npx, dataset, root->weights);
    
    sem = sem_open("THREADS_CONTROL", O_CREAT, 0600, 0);
    int train_weights_count = root->npx->size - 1;
    int active_update_threads_count = (int)min(train_weights_count, root->npx->settings->threads);
    int iterations_in_seen = root->data_set->training_set[0]->count / root->npx->settings->batch;
    int i, j;
    
    int epoch = 0;
    root->params->prev_error = cross_validation(validation, 0);
    printf("Start: Cross Validation Error: %f\n", root->params->prev_error);
    while (root->params->prev_error > root->npx->settings->accuracy) {
        root->params->train_set_number = epoch % root->data_set->count;
        shuffle(root->data_set->training_set[root->params->train_set_number]);
        if (epoch == 0 && output_path != NULL) {
            char *initial_weights_path = malloc((strlen(output_path) +
                                                 strlen("0.npw") + 1) * sizeof(*initial_weights_path));
            strcpy(initial_weights_path, output_path);
            strcat(initial_weights_path, "0.npw");
            write_npw(initial_weights_path, root->weights, root->npx->net, root->npx->size - 1);
            free(initial_weights_path);
        }
        for (i = 0; i < iterations_in_seen; i++) {
            root->params->iteration_number = i;
            pthread_t compute_threads[root->npx->settings->batch];
            for (j = 0; j < root->npx->settings->batch; j++) {
                pthread_create(&(compute_threads[j]), NULL, compute_network, &root->networks[j]);
            }
            for (j = 0; j < root->npx->settings->threads; j++) {
                sem_post(sem);
            }
            for (j = 0; j < root->npx->settings->batch; j++) {
                pthread_join(compute_threads[j], NULL);
            }
            pthread_t update_threads[train_weights_count];
            for (j = 0; j < train_weights_count; j++) {
                pthread_create(&(update_threads[j]), NULL, update_weights, &root->weights[j]);
            }
            for (j = 0; j < active_update_threads_count; j++) {
                sem_post(sem);
            }
            for (j = 0; j < train_weights_count; j++) {
                pthread_join(update_threads[j], NULL);
            }
            int iteration = epoch*iterations_in_seen + i;
            if (iteration != 0 && iteration % npx->settings->save_frequency == 0 && output_path != NULL) {
                char buffer_path[4096];
                sprintf(buffer_path, "%s%d.npw", output_path, iteration);
                write_npw(buffer_path, root->weights, root->npx->net, root->npx->size - 1);
            }
            printf("Iteration: %d  Error: %f\n", iteration, average(root->params->batch_errors, root->npx->settings->batch));
        }
        float error = cross_validation(validation, root->params->train_set_number);
        epoch = epoch + 1;
        printf("Epoch: %d Cross Validation Error: %f\n", epoch, error);
        update_params(root->params, error);
    }
    sem_close(sem);
    free_train_root(root);
    free_validation_root(validation);
    free_npx_config(npx);
    free_input_data(raw_input_data);
    free_data_set(dataset);
    return NPX_SUCCESS;
}

int update_params(train_params *params, float error) {
    switch (params->learning) {
        case GRADIENT_DESCENT:
            if (error - params->gamma * params->prev_error > 0) {
                params->eta = params->eta * params->alpha;
            } else {
                params->eta = params->eta * params->beta;
            }
            printf("Updating params: deltaError: %f, learning rate: %f\n", error - params->gamma * params->prev_error, params->eta);
            params->prev_error = error;
            break;
        case NDEF_LEARN:
            printf(FATAL_ERROR_NDF_LEARNING_TYPE_FAIL_MSG);
            exit(1);
    }
    return 0;
}

data_set *make_data_set(int sets_count, int train_data_count, int validation_data_count) {
    data_set *set = malloc(sizeof(*set));
    set->count = sets_count;
    set->training_set = malloc(set->count * sizeof(*set->training_set));
    set->validation_set = malloc(set->count * sizeof(*set->validation_set));
    int i;
    for (i = 0; i < set->count; ++i) {
        set->training_set[i] = make_input_data(train_data_count);
        set->validation_set[i] = make_input_data(validation_data_count);
    }
    return set;
}

int free_data_set(data_set *set) {
    free(set->training_set);
    free(set->validation_set);
    free(set);
    return 0;
}

data_set *prepare_data_set(input_data *raw_input_data, int batch_size) {
    int input_count = raw_input_data->count;
    int validation_rest = input_count % batch_size;
    
    int validation_data_count = batch_size + validation_rest;
    int train_data_count = input_count - validation_data_count;
    int sets_count = input_count / batch_size;
    
    data_set *set = make_data_set(sets_count, train_data_count, validation_data_count);
    
    int i, j;
    for (i = 0; i < set->count; ++i) {
        int training_set_index = 0;
        int validation_set_index = 0;
        int offset_lower_bound = i * batch_size;
        int offset_higher_bound = offset_lower_bound + batch_size;
        for (j = 0; j < input_count; ++j) {
            /// If the item is outside validation set bound it goes to training set
            /// otherwise it goes to validation set.
            if ( (j < offset_lower_bound || j >= offset_higher_bound) && training_set_index != train_data_count) {
                set->training_set[i]->items[training_set_index] = raw_input_data->items[j];
                training_set_index++;
            } else {
                set->validation_set[i]->items[validation_set_index] = raw_input_data->items[j];
                validation_set_index++;
            }
        }
    }
    return set;
}

train_network make_train_network(npx_config *npx, data_set *data, train_weights *weights, int position_in_batch, int *iteration_number, int *train_set_number, float *error) {
    train_network net;
    net.dataset = data;
    net.batch = npx->settings->batch;
    net.input_length = npx->settings->input_length;
    net.target_length = npx->settings->target_length;
    net.iteration_number = iteration_number;
    net.train_set_number = train_set_number;
    net.position_in_batch = position_in_batch;
    net.count = npx->size;
    net.layers = malloc(npx->size * sizeof(*net.layers));
    float *prev_gradients = NULL;
    float *prev_output_derivative = NULL;
    float *prev_output = NULL;
    for (int i = 0; i < npx->size - 1; i++) {
        switch (npx->net[i].type) {
            case CONVOLUTIONAL:
                net.layers[i] = make_convolutional_layer(npx->net[i], weights[i].values, prev_gradients, prev_output_derivative, prev_output, weights[i].corrections[position_in_batch]);
                prev_gradients = ((convolutional_layer*)net.layers[i])->gradients;
                prev_output_derivative = ((convolutional_layer*)net.layers[i])->output_derivative;
                prev_output = ((convolutional_layer*)net.layers[i])->output;
                break;
            case CONNECTED:
                net.layers[i] = make_connected_layer(npx->net[i], weights[i].values, prev_gradients, prev_output_derivative, prev_output, weights[i].corrections[position_in_batch]);
                prev_gradients = ((connected_layer*)net.layers[i])->gradients;
                prev_output_derivative = ((connected_layer*)net.layers[i])->output_derivative;
                prev_output = ((connected_layer*)net.layers[i])->output;
                break;
            default:
                printf(FATAL_ERROR_MAKE_NETWORK_FAIL_MSG);
                exit(1);
        }
    }
    loss_mode mode = detect_loss_mode(npx->net[npx->size - 2]);
    net.layers[npx->size - 1] = make_loss_layer(npx->net[npx->size - 1], prev_gradients, prev_output, prev_output_derivative, error, mode);
    return net;
}

loss_mode detect_loss_mode(layer_config config) {
    loss_mode mode;
    switch (config.activation) {
        case SOFTMAX:
            mode = SOFTMAX_MODE;
            break;
        case NDEF_ACTIVATION:
            mode = NDEF_LOSS_MODE;
            break;
        default:
            mode = SCALAR_DERIVATIVE_MODE;
            break;
    }
    return mode;
}

int clean_train_network(train_network net) {
    for (int i = 0; i < net.count - 1; i++) {
        int is_first_layer = i == 0;
        free_layer(net.layers[i], is_first_layer);
    }
    free_loss_layer(net.layers[net.count - 1]);
    free(net.layers);
    return 0;
}

train_root *make_train_root(npx_config *npx, data_set *data_set) {
    train_root *root = malloc(sizeof(*root));
    root->npx = npx;
    root->data_set = data_set;
    root->params = make_train_params(npx->settings);
    root->weights = malloc((npx->size - 1) * sizeof(*root->weights));
    for (int i = 0; i < npx->size - 1; i++) {
        root->weights[i] = make_train_weights(root->npx->net[i], root->params);
    }
    root->networks = malloc(npx->settings->batch * sizeof(*root->networks));
    for (int i = 0; i < npx->settings->batch; i++) {
        root->networks[i] = make_train_network(npx, data_set, root->weights, i, &root->params->iteration_number, &root->params->train_set_number, &root->params->batch_errors[i]);
    }
    return root;
}

int free_train_root(train_root *root) {
    for (int i = 0; i < root->npx->settings->batch; i++) {
        clean_train_network(root->networks[i]);
    }
    free(root->networks);
    for (int i = 0; i < root->npx->size - 1; i++) {
        clean_train_weights(root->weights[i]);
    }
    free(root->weights);
    free_train_params(root->params);
    free(root);
    return 0;
}

train_params *make_train_params(settings_config *settings) {
    train_params *params = malloc(sizeof(*params));
    params->alpha = settings->alpha;
    params->batch = settings->batch;
    params->beta = settings->beta;
    params->eta = settings->eta;
    params->gamma = settings->gamma;
    params->lambda = settings->lambda;
    params->learning = settings->learning;
    params->momentum = settings->momentum;
    params->prev_error = 0;
    params->iteration_number = 0;
    params->train_set_number = 0;
    params->regularization = settings->regularization;
    params->batch_errors = calloc(settings->batch, sizeof(*params->batch_errors));
    return params;
}

int free_train_params(train_params *params) {
    free(params->batch_errors);
    free(params);
    return 0;
}

train_weights make_train_weights(layer_config config, train_params *params) {
    train_weights weights;
    weights.type = WEIGHT_TRAIN_TYPE;
    weights.params = params;
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
            break;
    }
    weights.values = calloc(weights.count, sizeof(*weights.values));
    weights.prev_delta = calloc(weights.count, sizeof(*weights.prev_delta));
    weights.corrections = malloc(weights.params->batch * sizeof(*weights.corrections));
    for (int i = 0; i < weights.params->batch; i++) {
        weights.corrections[i] = calloc(weights.count, sizeof(**weights.corrections));
    }
    return weights;
}

int clean_train_weights(train_weights weights) {
    for (int i = 0; i < weights.params->batch; i++) {
        free(weights.corrections[i]);
    }
    free(weights.corrections);
    free(weights.values);
    return 0;
}

int init_weights(train_weights *weights, npx_config *npx) {
    for (int i = 0; i < npx->size - 1; i++) {
        switch (npx->net[i].type) {
            case CONNECTED:
                switch (npx->settings->init) {
                    case XAVIER:
                        xavier_init_connected_weights(weights[i].values, npx->net[i]);
                        break;
                    case NDEF_INITIALIZATION:
                        printf(FATAL_ERROR_INIT_CONNECTED_WEIGHTS_FAIL_MSG);
                        exit(1);
                        break;
                }
                break;
            case CONVOLUTIONAL:
                switch (npx->settings->init) {
                    case XAVIER:
                        xavier_init_convolutional_weights(weights[i].values, npx->net[i]);
                        break;
                    case NDEF_INITIALIZATION:
                        printf(FATAL_ERROR_INIT_CONVOLUTIONAL_WEIGHTS_FAIL_MSG);
                        exit(1);
                        break;
                }
                break;
            case LOSS:
                printf(FATAL_ERROR_INIT_WEIGHTS_FAIL_MSG, LOSS_LAYER_KEY);
                exit(1);
                break;
            case NDEF_LAYER:
                printf(FATAL_ERROR_INIT_WEIGHTS_FAIL_MSG, "[NOT DEFINED]");
                exit(1);
                break;
        }
    }
    return 0;
}

int validation_size(int raw_input_data_size, int validation) {
    int train_data_count = raw_input_data_size / validation;
    int size = train_data_count > 0 ? train_data_count : 1;
    return size;
}

int xavier_init_connected_weights(float *weights, layer_config config) {
    float r = sqroot( 1.0 / (config.input_length + config.output_length));
    int length = config.input_length * config.output_length;
    for (int i = 0; i < length; i++) {
        weights[i] = rand_uniform(-r, r);
    }
    return 0;
}

int xavier_init_convolutional_weights(float *weights, layer_config config) {
    int input = config.input_width * config.input_height * config.input_depth;
    int output = config.output_width * config.output_height * config.channels;
    float r = sqroot( 6.0 / (input + output) );
    int length = config.width * config.height * config.input_depth * config.channels;
    for (int i = 0; i < length; ++i) {
        weights[i] = rand_uniform(-r, r);
    }
    return 0;
}

int validate_data(input_data *data, int input_length, int target_length, int batch_size, int validation_items_count) {
    if ( (data->count - validation_items_count) / batch_size == 0) {
        printf(ERROR_NOT_ENOUGH_DATA_MSG, data->count, batch_size);
        return ERROR_FORMAT;
    }
    float *input = malloc(input_length * sizeof(*input));
    float *target = malloc(target_length * sizeof(*target));
    for (int i = 0; i< data->count; i++) {
        if (read_npt(data->items[i], input, target, input_length, target_length) < 0) {
            free(input);
            free(target);
            return ERROR_FORMAT;
        }
    }
    free(input);
    free(target);
    return 0;
}

int read_npt(char *path, float *input, float *target, int input_length, int target_length) {
    FILE *file = fopen(path, "rb");
    if (!file) {
        printf(ERROR_OPEN_FILE_MSG, path);
        return ERROR_OPEN;
    }
    size_t success = fread(input, sizeof(*input), input_length, file);
    if (success != input_length) {
        printf(FATAL_ERROR_NPT_INPUT_FORMAT_FAIL_MSG, input_length, (int)success, path);
        return ERROR_FORMAT;
    }
    
    //TODO: Remove this shit
    //------------------------------
    
    for (int i = 0; i < input_length; i++) {
        input[i] = input[i] / 255.0;
    }
    
    //------------------------------
    
    success = fread(target, sizeof(*target), target_length, file);
    if (success != target_length) {
        printf(FATAL_ERROR_NPT_TARGET_FORMAT_FAIL_MSG, target_length, (int)success, path);
        return ERROR_FORMAT;
    }
    fclose(file);
    return 0;
}

input_data *read_train_data(char *path) {
    int npt_count = number_of_items_at_path(path);
    if (npt_count <= 0) {
        return make_input_data(0);
    }
    input_data *data = make_input_data(npt_count);

    DIR *train_dir = opendir(path);
    struct dirent *directory;
    int index = 0;
    while((directory = readdir(train_dir)) != NULL) {
        char *ext = strrchr(directory->d_name, '.');
        if (strcmp(ext, ".npt") == 0) {
            char item_path[255];
            sprintf(item_path, "%s/%s", path, directory->d_name);
            data->items[index] = malloc((strlen(item_path) + 1) * sizeof(*data->items[index]));
            strcpy(data->items[index], item_path);
            index++;
        }
    }
    closedir(train_dir);
    return data;
}

int shuffle(input_data *data) {
    int limit = data->count - 1;
    int rand_index = 0;
    char *tmp = NULL;
    for (int i = 0; i < data->count - 1; i++) {
        rand_index = rand_int(i, limit);
        tmp = data->items[i];
        data->items[i] = data->items[rand_index];
        data->items[rand_index] = tmp;
    }
    return 0;
}

input_data *make_input_data(int items_count) {
    input_data *data = malloc(sizeof(*data));
    data->count = items_count;
    data->items = malloc(items_count * sizeof(*data->items));
    return data;
}

int free_input_data(input_data *data) {
    for (int i = 0; i < data->count; i++) {
        free(data->items[i]);
    }
    free(data->items);
    free(data);
    return 0;
}

int number_of_items_at_path(char *path) {
    int count = 0;
    DIR *trainDir = opendir(path);
    if (trainDir == NULL) {
        return count;
    }
    struct dirent *directory;
    while((directory = readdir(trainDir)) != NULL) {
        char *ext = strrchr(directory->d_name, '.');
        if (strcmp(ext, ".npt") == 0) {
            count++;
        }
    }
    closedir(trainDir);
    return count;
}

int write_npw(char *path, train_weights *weights, layer_config *configs, int weights_count) {
    FILE *file = fopen(path, "wb");
    if (!file) {
        printf(ERROR_OPEN_FILE_MSG, path);
        return ERROR_OPEN;
    }
    fwrite(&weights_count, sizeof(weights_count), 1, file);
    int i;
    for (i = 0; i < weights_count; ++i) {
        train_weights weight = weights[i];
        layer_config config = configs[i];
        fwrite(&config, sizeof(config), 1, file);
        fwrite(weight.values, sizeof(*weight.values), weight.count, file);
    }
    
    fclose(file);
    return 0;
}

int read_npw(char *path, void *weights, int weights_count) {
    FILE *file = fopen(path, "rb");
    if (!file) {
        printf(ERROR_OPEN_FILE_MSG, path);
        return ERROR_OPEN;
    }

    fseek(file, sizeof(int), SEEK_CUR);
    weight_type type = *(weight_type*)weights;
    int i;
    size_t success = 0;
    for (i = 0; i < weights_count; ++i) {
        fseek(file, sizeof(layer_config), SEEK_CUR);
        int weight_values_count = 0;
        float *weights_values;
        switch (type) {
            case WEIGHT_TRAIN_TYPE: {
                train_weights weight = ((train_weights *)weights)[i];
                weights_values = weight.values;
                weight_values_count = weight.count;
            }
                break;
            case WEIGHT_RUN_TYPE: {
                run_weights weight = ((run_weights *)weights)[i];
                weights_values = weight.values;
                weight_values_count = weight.count;
            }
                break;
        }
        success = fread(weights_values, sizeof(*weights_values), weight_values_count, file);
        if (success != weight_values_count) {
            printf(FATAL_ERROR_NPW_VALUES_FORMAT_FAIL_MSG, weight_values_count, (int)success, path);
            fclose(file);
            return ERROR_FORMAT;
        }
    }
    fclose(file);
    return 0;
}
