//
//  test_loss.c
//  test
//
//  Created by Pavel Kondrashkov on 5/28/18.
//  Copyright © 2018 Touchlane LLC. All rights reserved.
//

#include "test_loss.h"
#include <stdlib.h>
#include "unit_test.h"
#include "config.h"
#include "loss.h"

layer_config stub_loss_config(loss_type type);

int test_loss_make_loss_layer(void) {
    layer_config config = stub_loss_config(MSQE);
    float input[] = { 0.25, 0.75 };
    float *previous_gradient = calloc(config.output_length, sizeof(*previous_gradient));
    float *input_derivative = calloc(config.input_length, sizeof(*input_derivative));
    float *error = calloc(1, sizeof(*error));
    
    loss_layer *layer = make_loss_layer(config, previous_gradient, input, input_derivative, error, SCALAR_DERIVATIVE_MODE);
    
    assert_equal_float(layer->input[0], input[0]);
    assert_equal_float(layer->input[1], input[1]);
    assert_equal_ptr(layer->input, input);
    assert_equal_ptr(layer->input_derivative, input_derivative);
    
    assert_equal(layer->type, LOSS);
    assert_equal(layer->loss, config.loss);
    assert_not_equal(layer->loss, NDEF_LOSS);
    
    assert_equal(layer->size, config.input_length);
    assert_equal(layer->mode, SCALAR_DERIVATIVE_MODE);
    
    assert_equal(layer->target[0], 0);
    assert_equal(layer->target[1], 0);
    
    assert_equal_ptr(layer->previous_gradients, previous_gradient);
    assert_equal_ptr(layer->error, error);
    
    free(previous_gradient);
    free(input_derivative);
    free(error);
    free_loss_layer(layer);
    
    return 0;
}

int test_loss_forward(void) {
    layer_config config = stub_loss_config(MSQE);
    float input[] = { 0.25, 0.75 };
    float *previous_gradient = calloc(config.output_length, sizeof(*previous_gradient));
    float *input_derivative = calloc(config.input_length, sizeof(*input_derivative));
    float *error = calloc(1, sizeof(*error));
    
    loss_layer *layer = make_loss_layer(config, previous_gradient, input, input_derivative, error, SCALAR_DERIVATIVE_MODE);
    layer->target[0] = 0;
    layer->target[1] = 1;
    
    loss_forward(layer);
    
    assert_equal_float(*layer->error, 0.0625);
    
    free(previous_gradient);
    free(input_derivative);
    free(error);
    free_loss_layer(layer);
    
    return 0;
}

int test_loss_backward_scalar_msqe(void) {
    layer_config config = stub_loss_config(MSQE);
    float input[] = { 0.909297427, -0.544921111 }; // sin(2), sin(10)
    float *previous_gradient = calloc(config.output_length, sizeof(*previous_gradient));
    float input_derivative[] = { -0.416146837, -0.839071529 }; // cos(2), cos(10)
    float *error = calloc(1, sizeof(*error));
    
    loss_layer *layer = make_loss_layer(config, previous_gradient, input, input_derivative, error, SCALAR_DERIVATIVE_MODE);
    layer->target[0] = 0.1;
    layer->target[1] = 0.9;
    
    loss_backward(layer);

    assert_equal_float(layer->previous_gradients[0], -0.3367865644);
    assert_equal_float(layer->previous_gradients[1], 1.2123921659);

    free(previous_gradient);
    free(error);
    free_loss_layer(layer);
    
    return 0;
}

int test_loss_backward_softmax_msqe(void) {
    layer_config config = stub_loss_config(MSQE);
    float input[] = { 0.909297427, -0.544921111 }; // sin(2), sin(10)
    float *previous_gradient = calloc(config.output_length, sizeof(*previous_gradient));
    float input_derivative[] = { -0.416146837, -0.839071529 }; // cos(2), cos(10)
    float *error = calloc(1, sizeof(*error));
    
    loss_layer *layer = make_loss_layer(config, previous_gradient, input, input_derivative, error, SOFTMAX_MODE);
    layer->target[0] = 0.1;
    layer->target[1] = 0.9;
    
    loss_backward(layer);
    
    assert_equal_float(layer->previous_gradients[0], 0.46775037);
    assert_equal_float(layer->previous_gradients[1], 0.50046974);
    
    free(previous_gradient);
    free(error);
    free_loss_layer(layer);
    
    return 0;
}

int test_loss_backward_scalar_cross_entropy(void) {
    layer_config config = stub_loss_config(CROSS_ENTROPY);
    float input[] = { 0.909297427, -0.544921111 }; // sin(2), sin(10)
    float *previous_gradient = calloc(config.output_length, sizeof(*previous_gradient));
    float input_derivative[] = { -0.416146837, -0.839071529 }; // cos(2), cos(10)
    float *error = calloc(1, sizeof(*error));

    loss_layer *layer = make_loss_layer(config, previous_gradient, input, input_derivative, error, SCALAR_DERIVATIVE_MODE);
    layer->target[0] = 0.1;
    layer->target[1] = 0.9;
    
    loss_backward(layer);
    
    assert_equal_float(layer->previous_gradients[0], 0.0457657555);
    assert_equal_float(layer->previous_gradients[1], -1.38582331);
    
    free(previous_gradient);
    free(error);
    free_loss_layer(layer);
    
    return 0;
}

int test_loss_backward_softmax_cross_entropy(void) {
    layer_config config = stub_loss_config(CROSS_ENTROPY);
    float input[] = { 0.909297427, -0.544921111 }; // sin(2), sin(10)
    float *previous_gradient = calloc(config.output_length, sizeof(*previous_gradient));
    float input_derivative[] = { -0.416146837, -0.839071529 }; // cos(2), cos(10)
    float *error = calloc(1, sizeof(*error));

    loss_layer *layer = make_loss_layer(config, previous_gradient, input, input_derivative, error, SOFTMAX_MODE);
    layer->target[0] = 0.1;
    layer->target[1] = 0.9;
    
    loss_backward(layer);
    
    assert_equal_float(layer->previous_gradients[0], 0.8092973);
    assert_equal_float(layer->previous_gradients[1], -1.444921);
    
    free(previous_gradient);
    free(error);
    free_loss_layer(layer);
    
    return 0;
}

layer_config stub_loss_config(loss_type type) {
    layer_config config;
    config.type = LOSS;
    config.loss = type;
    config.input_length = 2;
    config.output_length = 2;
    
    return config;
}
