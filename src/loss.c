//
//  loss.c
//  netapix
//
//  Created by Evgeny Dedovets on 1/11/18.
//  Copyright © 2018 Touchlane LLC. All rights reserved.
//

#include "loss.h"
#include "math.h"
#include "constants.h"
#include <stdlib.h>
#include <stdio.h>

loss_layer *make_loss_layer(layer_config config, float *previous_gradients, float *input, float *input_derivative, float *error, loss_mode mode) {
    loss_layer *layer = malloc(sizeof(*layer));
    layer->error = error;
    layer->loss = config.loss;
    layer->size = config.input_length;
    layer->previous_gradients = previous_gradients;
    layer->input = input;
    layer->input_derivative = input_derivative;
    layer->type = LOSS;
    layer->target = calloc(config.input_length, sizeof(*layer->target));
    layer->mode = mode;
    return layer;
}

int free_loss_layer(loss_layer *layer) {
    free(layer->target);
    free(layer);
    return 0;
}

int loss_forward(loss_layer *layer) {
    *layer->error = 0;
    switch (layer->loss) {
        case MSQE:
            *layer->error = msqe(layer->input, layer->target, layer->size);
            break;
        case CROSS_ENTROPY:
            *layer->error = cross_entropy(layer->input, layer->target, layer->size);
            break;
        case NDEF_LOSS:
            printf(FATAL_ERROR_LOSS_FAIL_MSG);
            exit(1);
    }
    return 0;
}

int loss_backward(loss_layer *layer) {
    int i, j;
    for (i = 0; i < layer->size; i++) {
        layer->previous_gradients[i] = 0;
        switch (layer->loss) {
            case MSQE:
                switch (layer->mode) {
                    case SCALAR_DERIVATIVE_MODE:
                        layer->previous_gradients[i] = layer->input_derivative[i] * (layer->input[i] - layer->target[i]);
                        break;
                    case SOFTMAX_MODE:
                        for (j = 0; j < layer->size; j++) {
                            if (j == i) {
                                layer->previous_gradients[i] = layer->previous_gradients[i] + layer->input[i] * (layer->input[i] - layer->target[i]) * (1 - layer->input[i]);
                            } else {
                                layer->previous_gradients[i] = layer->previous_gradients[i] + (layer->target[i] - layer->input[i]) * layer->input[i] * layer->input[j];
                            }
                        }
                        break;
                    case NDEF_LOSS_MODE:
                        printf(FATAL_ERROR_LOSS_FAIL_MSG);
                        exit(1);
                        break;
                }
                break;
            case CROSS_ENTROPY:
                switch (layer->mode) {
                    case SOFTMAX_MODE:
                        layer->previous_gradients[i] = layer->input[i] - layer->target[i];
                        break;
                    default:
                        printf(FATAL_ERROR_LOSS_FAIL_MSG);
                        exit(1);
                        break;
                }
                break;
            case NDEF_LOSS:
                printf(FATAL_ERROR_LOSS_FAIL_MSG);
                exit(1);
                break;
        }
    }
    return 0;
}


