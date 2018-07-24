//
//  test_run.c
//  test
//
//  Created by Pavel Kondrashkov on 6/6/18.
//  Copyright © 2018 Touchlane LLC. All rights reserved.
//

#include "test_run.h"
#include <stdlib.h>
#include "unit_test.h"
#include "run.h"
#include "connected.h"

layer_config *stub_layer_configs(void);

int test_run_make_run_root(void) {
    layer_config *configs = stub_layer_configs();
    int layers_count = 2;
    int input_size = 4;
    float *input_data = malloc(input_size * sizeof(*input_data));

    run_root *root = make_run_root(configs, layers_count, input_data);
    
    assert_equal_ptr(root->input_data, input_data);
    assert_not_equal(root->weights, NULL);
    assert_not_equal(root->weights[root->network.count - 1].count, 0);
    assert_not_equal(root->weights[root->network.count - 1].values, NULL);
    
    free_run_root(root);
    free(configs);
    
    return 0;
}

int test_run_free_run_root(void) {
    layer_config *configs = stub_layer_configs();
    int layers_count = 2;
    int input_size = 4;
    float *input_data = malloc(input_size * sizeof(*input_data));
    run_root *root = make_run_root(configs, layers_count, input_data);
    
    int result = free_run_root(root);
    
    assert_equal(result, 0);
    
    free(configs);
    
    return 0;
}

int test_run_make_run_weights(void) {
    layer_config *configs = stub_layer_configs();

    run_weights weights = make_run_weights(configs[0]);
    int expected_values_count = (configs[0].input_length + 1) * configs[0].output_length;
    
    assert_equal(weights.count, expected_values_count);
    assert_not_equal(weights.values, NULL);
    
    free_run_weights(weights);
    free(configs);
    
    return 0;
}

int test_run_free_run_weights(void) {
    layer_config *configs = stub_layer_configs();
    run_weights weights = make_run_weights(configs[0]);

    int result = free_run_weights(weights);
    
    assert_equal(result, 0);
    
    free(configs);
    
    return 0;
}

int test_run_make_network(void) {
    layer_config *configs = stub_layer_configs();
    int layers_count = 2;
    run_weights *weights = malloc((layers_count) * sizeof(*weights));
    int input_size = 4;
    float *input_data = malloc(input_size * sizeof(*input_data));

    run_network net = make_run_network(configs, layers_count, weights, input_data);
    
    float *prev_output = input_data;
    for (int i = 0; i < layers_count; i++) {
        switch (configs[i].type) {
            case CONVOLUTIONAL:
                break;
            case CONNECTED: {
                connected_layer *layer = net.layers[i];
                assert_equal_ptr(layer->input, prev_output);
                prev_output = layer->output;
            }
                break;
            default:
                fail();
        }
    }
    
    free_run_network(net);
    free(configs);
    
    return 0;
}

int test_run_free_network(void) {
    layer_config *configs = stub_layer_configs();
    int layers_count = 2;
    run_weights *weights = malloc((layers_count) * sizeof(*weights));
    int input_size = 4;
    float *input_data = malloc(input_size * sizeof(*input_data));
    run_network net = make_run_network(configs, layers_count, weights, input_data);

    int result = free_run_network(net);
    
    assert_equal(result, 0);
    
    free(configs);
    
    return 0;
}

int test_run_free_layer(void) {
    layer_config *configs = stub_layer_configs();
    int layers_count = 2;
    run_weights *weights = malloc((layers_count) * sizeof(*weights));
    int input_size = 4;
    float *input_data = malloc(input_size * sizeof(*input_data));
    run_network net = make_run_network(configs, layers_count, weights, input_data);

    int result = 0;
    int i;
    for (i = 0; i < net.count; ++i) {
        int is_first_layer = i == 0;
        result += free_layer(net.layers[i], is_first_layer);
    }
    
    assert_equal(result, 0);
    
    free(configs);
    
    return 0;
}

layer_config *stub_layer_configs(void) {
    int size = 2;
    layer_config *configs = malloc(size * sizeof(layer_config));

    configs[0] = make_layer_config();
    configs[0].type = CONNECTED;
    configs[0].input_length = 4;
    configs[0].output_length = 2;
    configs[0].activation = RELU;
    
    configs[1] = make_layer_config();
    configs[1].type = CONNECTED;
    configs[1].input_length = 2;
    configs[1].output_length = 1;
    configs[1].activation = SOFTMAX;
    return configs;
}
