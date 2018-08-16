//
//  convolutional.c
//  netapix
//
//  Created by Evgeny Dedovets on 6/7/18.
//  Copyright © 2018 Touchlane LLC. All rights reserved.
//

#include "convolutional.h"
#include <stdlib.h>
#include <math.h>
#include "utils.h"
#include "math.h"

convolutional_layer *make_convolutional_layer(layer_config config, float *weights, float *prev_gradients, float *input_derivative, float *input, float *corrections) {
    convolutional_layer *layer = malloc(sizeof(*layer));
    layer->p = config.padding;
    layer->s = config.stride;
    layer->h = config.input_height;
    layer->w = config.input_width;
    layer->z = config.input_depth;
    layer->w1 = config.width;
    layer->h1 = config.height;
    layer->z1 = config.input_depth;
    layer->h2 = config.output_height;
    layer->w2 = config.output_width;
    layer->z2 = config.channels;
    layer->type = config.type;
    layer->activation = config.activation;
    layer->output_length = config.output_length;
    if (input != NULL) {
        layer->input = input;
    } else {
        layer->input = calloc(config.input_depth*config.input_width*config.input_height, sizeof(*layer->input));
    }
    layer->input_tensor = make_tensor(config.input_height, config.input_width, config.input_depth, 0, layer->input);
    if (input_derivative != NULL) {
        layer->input_derivative_tensor = make_tensor(config.input_height, config.input_width, config.input_depth, 0, input_derivative);
    }
    layer->weights_tmp = weights;
    layer->weights = make_array_of_tensors(config.height, config.width, config.input_depth, config.channels, weights);
    layer->biases = &weights[config.channels*config.height*config.width*config.input_depth];
    layer->output = calloc(config.output_length, sizeof(*layer->output));
    if (corrections != NULL) {
        layer->corrections = make_array_of_tensors(config.height, config.width, config.input_depth, config.channels, corrections);
        layer->biases_corrections = &corrections[config.channels*config.height*config.width*config.input_depth];
    } else {
        layer->corrections = NULL;
        layer->biases_corrections = NULL;
    }
    layer->output_tensor = make_tensor(config.output_height, config.output_width, config.channels, 0, layer->output);
    layer->output_derivative = calloc(config.output_length, sizeof(*layer->output));
    layer->output_derivative_tensor = make_tensor(config.output_height, config.output_width, config.channels, 0, layer->output_derivative);
    if (prev_gradients != NULL) {
        layer->previous_gradients_tensor = make_tensor(config.input_height, config.input_width, config.input_depth, 0, prev_gradients);
    }
    layer->gradients = calloc(config.output_length, sizeof(*layer->output));
    layer->gradients_tensor = make_tensor(config.output_height, config.output_width, config.channels, 0, layer->gradients);
    return layer;
}

void convolutional_forward(convolutional_layer *layer) {
    int h1, w1, z1, h2, w2, z2;
    for (z2 = 0; z2 < layer->z2; z2++) {
        for (h2 = 0; h2 < layer->h2; h2++) {
            for (w2 = 0; w2 < layer->w2; w2++) {
                *layer->output_tensor[z2][h2][w2] = 0;
                for (z1 = 0; z1 < layer->z1; z1++) {
                    for (h1 = 0; h1 < layer->h1; h1++) {
                        for (w1 = 0; w1 < layer->w1; w1++) {
                            if (h2*layer->s + h1 - layer->p >= 0 && h2*layer->s + h1 - layer->p < layer->h && w2*layer->s + w1 - layer->p >= 0 && w2*layer->s + w1 - layer->p < layer->w) {
                                *layer->output_tensor[z2][h2][w2] = *layer->output_tensor[z2][h2][w2] + *layer->weights[z2][z1][h1][w1] * (*layer->input_tensor[z1][h2*layer->s + h1 - layer->p][w2*layer->s + w1 - layer->p]);
                            }
                        }
                    }
                }
                *layer->output_tensor[z2][h2][w2] = *layer->output_tensor[z2][h2][w2] + layer->biases[z2];
                if (layer->activation != SOFTMAX) {
                    *layer->output_derivative_tensor[z2][h2][w2] = derivative(*layer->output_tensor[z2][h2][w2], layer->activation);
                    activate(layer->output_tensor[z2][h2][w2], layer->activation);
                }
            }
        }
    }
    if (layer->activation == SOFTMAX) {
        softmax(layer->output, layer->output_length);
    }
}

void convolutional_backward(convolutional_layer *layer) {
    int h, w, z, h2, w2, z2;
    double h2BottomLimit, w2BottomLimit, h2TopLimit, w2TopLimit;
    for (z = 0; z < layer->z; z++) {
        for (h = 0; h < layer->h; h++) {
            for (w = 0; w < layer->w; w++) {
                *layer->previous_gradients_tensor[z][h][w] = 0;
                for (z2 = 0; z2 < layer->z2; z2++) {
                    h2BottomLimit = relu(ceil((double)(h + layer->p + 1 - layer->h1) / layer->s));
                    h2TopLimit = floor((double)(h + layer->p) / layer->s);
                    if (h2TopLimit > layer->h2 - 1) {
                        h2TopLimit = layer->h2 - 1;
                    }
                    for (h2 = h2BottomLimit; h2 <= h2TopLimit; h2++) {
                        w2BottomLimit = relu(ceil((float)(w + layer->p + 1 - layer->w1) / layer->s));
                        w2TopLimit = floor((double)(w + layer->p) / layer->s);
                        if (w2TopLimit > layer->w2 - 1) {
                            w2TopLimit = layer->w2 - 1;
                        }
                        for (w2 = w2BottomLimit; w2 <= w2TopLimit; w2++) {
                            *layer->previous_gradients_tensor[z][h][w] = *layer->previous_gradients_tensor[z][h][w] + *layer->gradients_tensor[z2][h2][w2] * *layer->weights[z2][z][h - h2 * layer->s + layer->p][w - w2 * layer->s + layer->p];
                        }
                    }
                }
                *layer->previous_gradients_tensor[z][h][w] = *layer->previous_gradients_tensor[z][h][w] * *layer->input_derivative_tensor[z][h][w];
            }
        }
    }
}

void calc_сonvolutional_сorrections(convolutional_layer *layer) {
    int h1, w1, z1, h2, w2, z2;
    for (z2 = 0; z2 < layer->z2; z2++) {
        for (z1 = 0; z1 < layer->z1; z1++) {
            for (h1 = 0; h1 < layer->h1; h1++) {
                for (w1 = 0; w1 < layer->w1; w1++) {
                    for (h2 = 0; h2 < layer->h2; h2++) {
                        for (w2 = 0; w2 < layer->w2; w2++) {
                            
                            if (h2*layer->s + h1 - layer->p >= 0 && h2*layer->s + h1 - layer->p < layer->h && w2*layer->s + w1 - layer->p >= 0 && w2*layer->s + w1 - layer->p < layer->w) {
                                *layer->corrections[z2][z1][h1][w1] = *layer->corrections[z2][z1][h1][w1] + *layer->gradients_tensor[z2][h2][w2] * *layer->input_tensor[z1][h2 * layer->s + h1 - layer->p][w2 * layer->s + w1 - layer->p];
                            }
                        }
                    }
                }
            }
        }
        for (h2 = 0; h2 < layer->h2; h2++) {
            for (w2 = 0; w2 < layer->w2; w2++) {
                layer->biases_corrections[z2] = layer->biases_corrections[z2] + *layer->gradients_tensor[z2][h2][w2];
            }
        }
    }
}

int free_convolutional_layer(convolutional_layer *layer, int is_first_layer) {
    if (is_first_layer) {
        free(layer->input);
        free_tensor(layer->previous_gradients_tensor, layer->h, layer->z);
        free_tensor(layer->input_derivative_tensor, layer->h, layer->z);
    }
    free_tensor(layer->input_tensor , layer->h, layer->z);
    free_array_of_tensors(layer->weights, layer->h1, layer->z1, layer->z2);
    free(layer->output);
    if (layer->corrections != NULL) {
        free_array_of_tensors(layer->corrections, layer->h1, layer->z1, layer->z2);
    }
    free_tensor(layer->output_tensor, layer->h2, layer->z2);
    free(layer->output_derivative);
    free_tensor(layer->output_derivative_tensor, layer->h2, layer->z2);
    free(layer->gradients);
    free_tensor(layer->gradients_tensor, layer->h2, layer->z2);
    free(layer);
    return 0;
}
