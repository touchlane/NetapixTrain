//
//  validation.c
//  netapix
//
//  Created by Pavel Kondrashkov on 6/11/18.
//  Copyright © 2018 Touchlane LLC. All rights reserved.
//

#include "validation.h"
#include <stdlib.h>
#include <pthread.h>
#include "math.h"
#include "constants.h"
#include "loss.h"
#include "connected.h"
#include "convolutional.h"
#include "train.h"

void *compute_validation_network(void *n) {
    validation_network *net = n;
    for (int i = 0; i < net->validation_items_count; ++i) {
        read_npt(net->validation_data->items[net->validation_offset + i],
                input_from_layer(net->layers[0]),
                ((loss_layer *) net->layers[net->count - 1])->target,
                net->input_length,
                net->target_length);

        for (int j = 0; j < net->count; j++) {
            forward(net->layers[j]);
        }
        (*net->batch_errors_ref)[net->validation_offset + i] = *net->error;
    }
    return NULL;
}

float cross_validation(validation_root *root, int iteration) {
    int validations_count = root->dataset->validation_set[iteration]->count;
    int max_concurrent_threads = root->networks_count;
    set_validation_offset(validations_count, root->networks, root->networks_count);

    pthread_t validate_threads[validations_count];
    for (int i = 0; i < max_concurrent_threads; i++) {
        root->networks[i].validation_data = root->dataset->validation_set[iteration];
        pthread_create(&(validate_threads[i]), NULL, compute_validation_network, &root->networks[i]);
    }
    for (int i = 0; i < max_concurrent_threads; i++) {
        pthread_join(validate_threads[i], NULL);
    }
    
    float error = average(root->batch_errors, validations_count);
    return error;
}

validation_root *make_validation_root(npx_config *npx, data_set *dataset, train_weights *weights) {
    validation_root *root = malloc(sizeof(*root));
    root->dataset = dataset;
    root->npx = npx;
    int validations_count = dataset->validation_set[0]->count;
    root->batch_errors = malloc(validations_count * sizeof(*root->batch_errors));
    
    root->networks_count = (int) min(validations_count, npx->settings->threads);
    validation_network *networks = malloc(root->networks_count * sizeof(*networks));

    for (int i = 0; i < root->networks_count; ++i) {
        networks[i] = make_validation_network(npx, weights, &root->batch_errors);
    }
    
    root->networks = networks;
    return root;
}

int free_validation_root(validation_root *root) {
    for (int i = 0; i < root->networks_count; ++i) {
        free_validation_network(root->networks[i]);
    }
    free(root->networks);
    free(root->batch_errors);
    free(root);
    return 0;
}

validation_network make_validation_network(npx_config *npx, train_weights *weights, float **batch_error_ref) {
    validation_network net;
    net.validation_data = NULL;
    net.validation_items_count = 0;
    net.input_length = npx->settings->input_length;
    net.target_length = npx->settings->target_length;
    net.error = calloc(1, sizeof(*net.error));
    net.batch_errors_ref = batch_error_ref;
    net.count = npx->size;

    net.layers = malloc(npx->size * sizeof(*net.layers));
    float *prev_output = NULL;
    for (int i = 0; i < npx->size - 1; i++) {
        switch (npx->net[i].type) {
            case CONVOLUTIONAL:
                net.layers[i] = make_convolutional_layer(npx->net[i],
                                                         weights[i].values,
                                                         NULL,
                                                         NULL,
                                                         prev_output,
                                                         NULL);
                prev_output = ((convolutional_layer*)net.layers[i])->output;
                break;
            case CONNECTED:
                net.layers[i] = make_connected_layer(npx->net[i],
                        weights[i].values,
                        NULL,
                        NULL,
                        prev_output,
                        NULL);
                prev_output = ((connected_layer*)net.layers[i])->output;
                break;
            default:
                printf(FATAL_ERROR_MAKE_NETWORK_FAIL_MSG);
                exit(1);
        }
    }
    loss_mode mode = detect_loss_mode(npx->net[npx->size - 2]);
    net.layers[npx->size - 1] = make_loss_layer(npx->net[npx->size - 1], NULL, prev_output, NULL, net.error, mode);
    return net;
}

int free_validation_network(validation_network net) {
    for (int i = 0; i < net.count - 1; ++i) {
        int is_first_layer = i == 0;
        free_layer(net.layers[i], is_first_layer);
    }
    free_loss_layer(net.layers[net.count - 1]);
    free(net.error);
    free(net.layers);
    return 0;
}

void set_validation_offset(int validation_data_count, validation_network *networks, int networks_count) {
    int items_for_validation = validation_data_count / networks_count;
    int rest = validation_data_count % networks_count;
    for (int i = 0; i < networks_count; i++) {
        networks[i].validation_items_count = items_for_validation;
        networks[i].validation_offset = items_for_validation * i;
    }
    networks[networks_count - 1].validation_items_count += rest;
}
