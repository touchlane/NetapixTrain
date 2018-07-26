//
//  connected.c
//  netapix
//
//  Created by Evgeny Dedovets on 1/11/18.
//  Copyright © 2018 Touchlane LLC. All rights reserved.
//

#include "connected.h"
#include "math.h"
#include <stdlib.h>
#include "utils.h"

connected_layer *make_connected_layer(layer_config config, float *weights, float *prev_gradients, float *input_derivative, float *input, float *corrections) {
    connected_layer *layer = malloc(sizeof(*layer));
    layer->type = config.type;
    layer->input_derivative = input_derivative;
    if (input != NULL) {
        layer->input = input;
    } else {
        layer->input = calloc(config.input_length, sizeof(*layer->input));
    }
    layer->weights = make_matrix(config.input_length, config.output_length, 1, 0, weights);
    layer->biases = &weights[config.output_length * config.input_length];
    layer->output = calloc(config.output_length, sizeof(*layer->output));
    layer->output_derivative = calloc(config.output_length, sizeof(*layer->output_derivative));
    if (corrections != NULL) {
        layer->corrections = make_matrix(config.input_length, config.output_length, 1, 0, corrections);
        layer->biases_corrections = &corrections[config.output_length * config.input_length];
    } else {
        layer->corrections = NULL;
        layer->biases_corrections = NULL;
    }
    layer->gradients = calloc(config.output_length, sizeof(*layer->gradients));
    layer->previous_gradients = prev_gradients;
    layer->activation = config.activation;
    layer->input_length = config.input_length;
    layer->output_length = config.output_length;
    return layer;
}

int free_connected_layer(connected_layer *layer, int is_first_layer) {
    free_matrix(layer->weights, layer->input_length);
    free(layer->output);
    free(layer->output_derivative);
    if (layer->corrections != NULL) {
        free_matrix(layer->corrections, layer->input_length);
    }
    free(layer->gradients);
    if (is_first_layer) {
        free(layer->input);
    }
    free(layer);
    return 0;
}

void connected_forward(connected_layer *layer) {
    int i, j;
    for (i = 0; i < layer->output_length; i++) {
        layer->output[i] = 0;
        for (j = 0; j < layer->input_length; j++) {
            layer->output[i] = layer->output[i] + layer->input[j] * *layer->weights[j][i];
        }
        layer->output[i] = layer->output[i] + layer->biases[i];
        if (layer->activation != SOFTMAX) {
            layer->output_derivative[i] = derivative(layer->output[i], layer->activation);
            activate(&layer->output[i], layer->activation);
        }
    }
    if (layer->activation == SOFTMAX) {
        softmax(layer->output, layer->output_length);
    }
}

void connected_backward(connected_layer *layer) {
    int i, j;
    for (i = 0; i < layer->input_length; i++) {
        layer->previous_gradients[i] = 0;
        for (j = 0; j < layer->output_length; j++) {
            layer->previous_gradients[i] = layer->previous_gradients[i] + layer->gradients[j] * *layer->weights[i][j];
        }
        layer->previous_gradients[i] = layer->previous_gradients[i] * layer->input_derivative[i];
    }
}

void calc_connected_corrections(connected_layer *layer) {
    int i, j;
    for(i = 0; i < layer->input_length; i++) {
        for (j = 0; j < layer->output_length; j++) {
            *layer->corrections[i][j] = *layer->corrections[i][j] + layer->input[i] * layer->gradients[j];
        }
    }
    for (i = 0; i < layer->output_length; i++) {
        layer->biases_corrections[i] = layer->biases_corrections[i] + layer->gradients[i];
    }
}
