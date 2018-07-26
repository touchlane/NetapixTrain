//
//  loss.h
//  netapix
//
//  Created by Evgeny Dedovets on 1/11/18.
//  Copyright © 2018 Touchlane LLC. All rights reserved.
//

#ifndef loss_h
#define loss_h

#include "config.h"

typedef struct {
    //Type of layer
    layer_type type;
    //Not activated input with applied delivative of activation function from previous layer.
    float *input_derivative;
    //Input from previous layer.
    float *input;
    //Target output value.
    float *target;
    //Number of channels.
    float size;
    //Loss value.
    float *error;
    //Error gradient.
    float *previous_gradients;
    //Loss type.
    loss_type loss;
    
    loss_mode mode;
} loss_layer;

loss_layer *make_loss_layer(layer_config config, float *previous_gradients, float *input, float *input_derivative, float *error, loss_mode mode);
int free_loss_layer(loss_layer *layer);
int loss_forward(loss_layer *layer);
int loss_backward(loss_layer *layer);

#endif /* loss_h */
