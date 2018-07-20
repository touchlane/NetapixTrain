//
//  connected.h
//  netapix
//
//  Created by Evgeny Dedovets on 1/11/18.
//  Copyright © 2018 Touchlane LLC. All rights reserved.
//

#ifndef connected_h
#define connected_h

#include "config.h"

typedef struct {
    //Type of layer.
    layer_type type;
    //Not activated input with applied delivative of activation function from previous layer.
    float *input_derivative;
    //Input array of channels.
    float *input;
    //Weights (width = number of outputs, heights = number of inputs).
    float ***weights;
    //Vector of bias values (size = number of outputs).
    float *biases;
    //Output array of channels (size = number of outputs).
    float *output;
    //Output with applied derivative of activation function instead of activation itself
    //(size = number of outputs).
    float *output_derivative;
    //Weights correction (width = number of outputs, heights = number of inputs).
    float ***corrections;
    //Batch bias corrections (size = number of outputs).
    float *biases_corrections;
    //Error gradients (size = number of outputs).
    float *gradients;
    //Previous layer error gradients (size = number of inputs).
    float *previous_gradients;
    //Activation type.
    activation_type activation;
    //Number of inputs.
    int input_length;
    //Number of outputs.
    int output_length;
} connected_layer;

connected_layer *make_connected_layer(layer_config config, float *weights, float *prev_gradients, float *input_derivative, float *input, float *corrections);
int free_connected_layer(connected_layer *layer, int is_first_layer);
void connected_forward(connected_layer *layer);
void connected_backward(connected_layer *layer);
void calc_connected_corrections(connected_layer *layer);

#endif /* connected_h */
