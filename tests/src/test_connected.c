//
//  test_connected.c
//  test
//
//  Created by Pavel Kondrashkov on 5/25/18.
//  Copyright © 2018 Touchlane LLC. All rights reserved.
//

#include "test_connected.h"
#include <stdlib.h>
#include "unit_test.h"
#include "config.h"
#include "connected.h"
#include "utils.h"

layer_config stub_connected_config(void);

int test_connected_make_connected_layer(void) {
    layer_config config = stub_connected_config();
    float weights[] = {
        0.1, 0.2, 0.3, 0.4,
        0.6, 0.3
    };
    float corrections[] = {
        0.1, 0.1, 0.4, 0.1,
        0.2, 0.1
    };
    float input[] = { 0.25, -0.2 };
    float *previous_gradient = calloc(config.output_length, sizeof(*previous_gradient));
    float *input_derivative = calloc(config.input_length, sizeof(*input_derivative));
    connected_layer *layer = make_connected_layer(config,
                                                  weights,
                                                  previous_gradient,
                                                  input_derivative,
                                                  input,
                                                  corrections);
    assert_equal_float(layer->input[0], input[0]);
    assert_equal_float(layer->input[1], input[1]);
    assert_equal_ptr(layer->input, input);
    assert_equal_ptr(layer->input_derivative, input_derivative);

    assert_equal_float(*layer->weights[0][0], weights[0]);
    assert_equal_float(*layer->weights[0][1], weights[1]);
    assert_equal_float(*layer->weights[1][0], weights[2]);
    assert_equal_float(*layer->weights[1][1], weights[3]);
    assert_equal_float(layer->biases[0], weights[4]);
    assert_equal_float(layer->biases[1], weights[5]);
    
    assert_equal_float(layer->output[0], 0);
    assert_equal_float(layer->output[1], 0);
    assert_equal_float(layer->output_derivative[0], 0);
    assert_equal_float(layer->output_derivative[1], 0);
    
    assert_equal_float(*layer->corrections[0][0], corrections[0]);
    assert_equal_float(*layer->corrections[0][1], corrections[1]);
    assert_equal_float(*layer->corrections[1][0], corrections[2]);
    assert_equal_float(*layer->corrections[1][1], corrections[3]);
    assert_equal_float(layer->biases_corrections[0], corrections[4]);
    assert_equal_float(layer->biases_corrections[1], corrections[5]);
    
    assert_equal_float(layer->gradients[0], 0);
    assert_equal_float(layer->gradients[1], 0);
    
    assert_equal_ptr(layer->previous_gradients, previous_gradient);
    
    assert_equal(layer->type, CONNECTED);
    assert_equal(layer->activation, config.activation);
    assert_not_equal(layer->activation, NDEF_ACTIVATION);
    assert_equal(layer->input_length, config.input_length);
    assert_equal(layer->output_length, config.output_length);
    
    free_connected_layer(layer, 0);
    free(previous_gradient);
    free(input_derivative);
    return 0;
}

int test_connected_forward(void) {
    layer_config config = stub_connected_config();
    float weights[] = {
        0.1, 0.2, 0.3, 0.4,
        0.6, 0.3
    };
    float corrections[] = {
        0.1, 0.1, 0.4, 0.1,
        0.2, 0.1
    };
    float input[] = { 0.25, -0.2 };
    float *previous_gradient = calloc(config.output_length, sizeof(*previous_gradient));
    float *input_derivative = calloc(config.input_length, sizeof(*input_derivative));
    
    connected_layer *layer = make_connected_layer(config,
                                                  weights,
                                                  previous_gradient,
                                                  input_derivative,
                                                  input,
                                                  corrections);
    
    connected_forward(layer);
    
    assert_equal_float(layer->output[0], 0.565);
    assert_equal_float(layer->output[1], 0.27);
    assert_equal_float(layer->output_derivative[0], 1);
    assert_equal_float(layer->output_derivative[1], 1);

    free_connected_layer(layer, 0);
    free(previous_gradient);
    free(input_derivative);
    
    return 0;
}

int test_connected_backward(void) {
    layer_config config = stub_connected_config();
    float weights[] = {
        0.1, 0.2, 0.3, 0.4,
        0.6, 0.3
    };
    float corrections[] = {
        0.1, 0.1, 0.4, 0.1,
        0.2, 0.1
    };
    float input[] = { 0, -0 };
    float *previous_gradient = calloc(config.output_length, sizeof(*previous_gradient));
    float *input_derivative = calloc(config.input_length, sizeof(*input_derivative));
    input_derivative[0] = 0.3;
    input_derivative[1] = 0.6;
    connected_layer *layer = make_connected_layer(config,
                                                  weights,
                                                  previous_gradient,
                                                  input_derivative,
                                                  input,
                                                  corrections);
    layer->gradients[0] = 0.2;
    layer->gradients[1] = -0.2;
    
    connected_backward(layer);
    
    assert_equal_float(layer->previous_gradients[0], -0.006);
    assert_equal_float(layer->previous_gradients[1], -0.012);
    
    free_connected_layer(layer, 0);
    free(previous_gradient);
    free(input_derivative);
    
    return 0;
}

int test_connected_calc_corrections(void) {
    layer_config config = stub_connected_config();
    float weights[] = {
        0.1, 0.2, 0.3, 0.4,
        0.6, 0.3
    };
    float corrections[] = {
        0.1, 0.1, 0.4, 0.1,
        0.2, 0.1
    };
    float input[] = { 0.25, -0.2 };
    float *previous_gradient = calloc(config.output_length, sizeof(*previous_gradient));
    float *input_derivative = calloc(config.input_length, sizeof(*input_derivative));
    connected_layer *layer = make_connected_layer(config,
                                                  weights,
                                                  previous_gradient,
                                                  input_derivative,
                                                  input,
                                                  corrections);
    layer->gradients[0] = 0.2;
    layer->gradients[1] = -0.2;
    
    calc_connected_corrections(layer);
    
    assert_equal_float(*layer->corrections[0][0], 0.15);
    assert_equal_float(*layer->corrections[0][1], 0.05);
    assert_equal_float(*layer->corrections[1][0], 0.36);
    assert_equal_float(*layer->corrections[1][1], 0.14);
    assert_equal_float(layer->biases_corrections[0], 0.4);
    assert_equal_float(layer->biases_corrections[1], -0.1);
    
    free_connected_layer(layer, 0);
    free(previous_gradient);
    free(input_derivative);
    
    return 0;
}

int test_connected_free_layer_first_layer(void) {
    layer_config config = stub_connected_config();
    float weights[] = {
        0.1, 0.2, 0.3, 0.4,
        0.6, 0.3
    };
    float corrections[] = {
        0.1, 0.1, 0.4, 0.1,
        0.2, 0.1
    };
    float *previous_gradient = calloc(config.output_length, sizeof(*previous_gradient));
    float *input_derivative = calloc(config.input_length, sizeof(*input_derivative));
    connected_layer *layer = make_connected_layer(config,
                                                  weights,
                                                  previous_gradient,
                                                  input_derivative,
                                                  NULL,
                                                  corrections);

    int is_first_layer = 1;
    int result = free_connected_layer(layer, is_first_layer);
    
    assert_equal(result, 0);
    
    free(previous_gradient);
    free(input_derivative);
    
    return 0;
}

int test_connected_free_layer_no_corrections(void) {
    layer_config config = stub_connected_config();
    float weights[] = {
        0.1, 0.2, 0.3, 0.4,
        0.6, 0.3
    };
    float *input = calloc(2, sizeof(*input));
    float *previous_gradient = calloc(config.output_length, sizeof(*previous_gradient));
    float *input_derivative = calloc(config.input_length, sizeof(*input_derivative));
    connected_layer *layer = make_connected_layer(config,
                                                  weights,
                                                  previous_gradient,
                                                  input_derivative,
                                                  input,
                                                  NULL);
    
    int is_first_layer = 0;
    int result = free_connected_layer(layer, is_first_layer);
    
    assert_equal(result, 0);
    
    free(input);
    free(previous_gradient);
    free(input_derivative);
    
    return 0;
}

layer_config stub_connected_config(void) {
    layer_config config;
    config.type = CONNECTED;
    config.input_length = 2;
    config.output_length = 2;
    config.activation = RELU;    
    return config;
}
