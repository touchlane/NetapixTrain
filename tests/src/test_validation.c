//
//  test_validation.c
//  test
//
//  Created by Pavel Kondrashkov on 6/25/18.
//  Copyright © 2018 Touchlane LLC. All rights reserved.
//

#include "test_validation.h"
#include "unit_test.h"
#include <stdlib.h>
#include "connected.h"
#include "validation.h"
#include "math.h"

npx_config *stub_validation_npx_config(void);
data_set *stub_data_set(void);
train_weights *stub_train_weights(npx_config *npx);

int test_validation_make_validation_root(void) {
    npx_config *npx = stub_validation_npx_config();
    data_set *dataset = stub_data_set();
    train_weights *weights = stub_train_weights(npx);
    validation_root *root = make_validation_root(npx, dataset, weights);
    
    int validations_count = dataset->validation_set[0]->count;
    int networks_count = (int)min(validations_count, npx->settings->threads);
    
    assert_not_equal(root->networks, NULL);
    assert_equal(root->networks_count, networks_count);
    assert_equal_ptr(root->npx, npx);
    assert_not_equal(root->batch_errors, NULL);
    assert_equal_ptr(root->dataset, dataset);
    
    return 0;
}

int test_validation_free_validation_root(void) {
    npx_config *npx = stub_validation_npx_config();
    data_set *dataset = stub_data_set();
    train_weights *weights = stub_train_weights(npx);
    validation_root *root = make_validation_root(npx, dataset, weights);
    
    int result = free_validation_root(root);
    
    assert_equal(result, 0);
    return 0;
}

int test_validation_make_validation_network(void) {
    npx_config *npx = stub_validation_npx_config();
    train_weights *weights = stub_train_weights(npx);
    float *batch_error_ref = malloc(npx->settings->batch * sizeof(batch_error_ref));
    
    validation_network net = make_validation_network(npx, weights, &batch_error_ref);
    
    assert_equal_ptr(net.batch_errors_ref, &batch_error_ref);;
    assert_not_equal(net.layers, NULL);
    assert_equal(net.input_length, npx->settings->input_length);
    assert_equal(net.target_length, npx->settings->target_length);
    assert_equal(net.count, npx->size);
    assert_equal(net.validation_items_count, 0);
    assert_equal(net.validation_offset, 0);
    assert_not_equal(net.error, NULL);
    
    return 0;
}

int test_validation_free_validation_network(void) {
    npx_config *npx = stub_validation_npx_config();
    train_weights *weights = stub_train_weights(npx);
    
    validation_network net = make_validation_network(npx, weights, NULL);
    int result = free_validation_network(net);
    
    assert_equal(result, 0);
    return 0;
}

int test_validation_set_validation_offset(void) {
    npx_config *npx = stub_validation_npx_config();
    train_weights *weights = stub_train_weights(npx);
    float *batch_error_ref = malloc(npx->settings->batch * sizeof(batch_error_ref));
    int networks_count = 4;
    validation_network networks[networks_count];
    for (int i = 0; i < networks_count; ++i) {
        networks[i] = make_validation_network(npx, weights, &batch_error_ref);
    }
    int validation_data_count = 6321;
    
    set_validation_offset(validation_data_count, networks, networks_count);
    
    
    
    assert_equal(networks[0].validation_items_count, 1580);
    assert_equal(networks[0].validation_offset, 0);
    assert_equal(networks[1].validation_items_count, 1580);
    assert_equal(networks[1].validation_offset, 1580);
    assert_equal(networks[2].validation_items_count, 1580);
    assert_equal(networks[2].validation_offset, 1580 + 1580);
    assert_equal(networks[3].validation_items_count, 1580 + 1);
    assert_equal(networks[3].validation_offset, 1580 + 1580 + 1580);
    int sum = 0;
    for (int i = 0; i < networks_count; ++i) {
        sum += networks[i].validation_items_count;
    }
    assert_equal(sum, validation_data_count);
    return 0;
}

npx_config *stub_validation_npx_config(void) {
    npx_config *npx = make_npx_config();
    npx->size = 2;
    layer_config *net_configs = malloc(npx->size * sizeof(layer_config));
    net_configs[0] = make_layer_config();
    net_configs[0].type = CONNECTED;
    net_configs[0].input_length = 4;
    net_configs[0].output_length = 2;
    net_configs[0].activation = RELU;
    
    net_configs[1] = make_layer_config();
    net_configs[1].type = CONNECTED;
    net_configs[1].input_length = 2;
    net_configs[1].output_length = 1;
    net_configs[1].activation = SOFTMAX;
    npx->net = net_configs;
    
    settings_config *settings = make_settings_config();
    settings->threads = 1;
    settings->input_length = net_configs[0].input_length;
    settings->target_length = net_configs[1].output_length;
    npx->settings = settings;
    
    return npx;
}

data_set *stub_data_set(void) {
    int raw_data_count = 11;
    input_data *raw_input_data = make_input_data(raw_data_count);
    for (int i = 0; i < raw_data_count; i++) {
        raw_input_data->items[i] = malloc(3 * sizeof(*raw_input_data->items[i]));
        sprintf(raw_input_data->items[i], "%02d", i);
    }
    int batch_size = validation_size(raw_data_count, 2);
    data_set *dataset = prepare_data_set(raw_input_data, batch_size);
    return dataset;
}

train_weights *stub_train_weights(npx_config *npx) {
    train_params *params = malloc(sizeof(*params));
    params->batch = 1;
    train_weights *weights = malloc((npx->size - 1) * sizeof(*weights));
    for (int i = 0; i < npx->size - 1; i++) {
        weights[i] = make_train_weights(npx->net[i], params);
    }
    return weights;
}
