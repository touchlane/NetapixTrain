//
//  convolutional.h
//  netapix
//
//  Created by Evgeny Dedovets on 6/7/18.
//  Copyright © 2018 Touchlane LLC. All rights reserved.
//

#ifndef convolutional_h
#define convolutional_h

#include "config.h"

/**
 * Defines convolutional layer structure.
 */
typedef struct {
    //Type of layer
    layer_type type;
    
    //Input from previous layer.
    float *input;
    
    float ****input_tensor;
    
    float ****input_derivative_tensor;

    float *weights_tmp;
    //Array of filters at the specific convolutional layer.
    float *****weights;
   
    float *****corrections;
    
    //Array of biases.
    float *biases;
    
    //Array of biases batch corrections.
    float *biases_corrections;
    
    //Output array of feature maps.
    float *output;
    
    int output_length;
    
    float ****output_tensor;
    
    float *output_derivative;
    
    //Output tensor with applied derivative of activation function instead of activation itself.
    float ****output_derivative_tensor;
    
    //Previous layer error gradient.
    
    float ****previous_gradients_tensor;
    
    //Array of gradients.
    float *gradients;
    
    float ****gradients_tensor;
    
    //Activation type.
    activation_type activation;
    
    //Width of input tensor.
    int w;
    
    //Height of input tensor.
    int h;

    //Depth of input tensor.
    int z;
    
    //Width of filter tensor.
    int w1;
    
    //Height of filter tensor.
    int h1;
    
    //Depth of input tensor.
    int z1;
    
    //Width of output tensor.
    int w2;
    
    //Height of output tensor.
    int h2;
    
    //Number of filters.
    int z2;
    
    //Stride
    int s;
    
    // padding
    int p;
    
} convolutional_layer;

convolutional_layer *make_convolutional_layer(layer_config config, float *weights, float *prev_gradients, float *input_derivative, float *input, float *corrections);
int free_convolutional_layer(convolutional_layer *layer, int is_first_layer);
void convolutional_forward(convolutional_layer *layer);
void convolutional_backward(convolutional_layer *layer);
void calc_сonvolutional_сorrections(convolutional_layer *layer);

#endif /* convolutional_h */
