//
//  test_training.c
//  test
//
//  Created by Pavel Kondrashkov on 5/31/18.
//  Copyright © 2018 Touchlane LLC. All rights reserved.
//

#include "test_training.h"
#include <stdlib.h>
#include <stdio.h>
#include "unit_test.h"
#include "train.h"
#include "config.h"

int test_training_validation_data_size(void) {
    int raw_input_data_size = 100;
    int validation_percentage = 20;

    int result = validation_size(raw_input_data_size, validation_percentage);
    
    assert_equal(result, 5);
    
    return 0;
}

int test_training_validation_data_size_min(void) {
    int raw_input_data_size = 11;
    int validation = 2;

    int result = validation_size(raw_input_data_size, validation);
    
    assert_equal(result, 5);
    
    return 0;
}

int test_training_prepare_data_set(void) {
    int raw_data_count = 11;
    input_data *raw_input_data = make_input_data(raw_data_count);
    for (int i = 0; i < raw_data_count; i++) {
        raw_input_data->items[i] = malloc(3 * sizeof(*raw_input_data->items[i]));
        sprintf(raw_input_data->items[i], "%02d", i);
    }
    int batch_size = validation_size(raw_data_count, 5);
    data_set *dataset = prepare_data_set(raw_input_data, batch_size);
    
    
    int rest = raw_data_count % batch_size;
    int validation_data_count = batch_size + rest;
    int train_data_count = raw_data_count - batch_size - rest;
    int sets_count = raw_input_data->count / batch_size;
    
    assert_equal(dataset->count, sets_count);
    assert_equal(dataset->training_set[0]->count, train_data_count);
    assert_equal(dataset->validation_set[0]->count, validation_data_count);

    assert_equal(dataset->training_set[dataset->count - 1]->count, train_data_count);
    assert_equal(dataset->validation_set[dataset->count - 1]->count, validation_data_count);

    assert_equal_string(dataset->validation_set[0]->items[0], "00");
    assert_equal_string(dataset->validation_set[0]->items[1], "01");
    assert_equal_string(dataset->validation_set[0]->items[2], "10");
    assert_equal_string(dataset->training_set[0]->items[0], "02");
    assert_equal_string(dataset->training_set[0]->items[1], "03");
    assert_equal_string(dataset->training_set[0]->items[2], "04");
    assert_equal_string(dataset->training_set[0]->items[3], "05");
    assert_equal_string(dataset->training_set[0]->items[4], "06");
    assert_equal_string(dataset->training_set[0]->items[5], "07");
    assert_equal_string(dataset->training_set[0]->items[6], "08");
    assert_equal_string(dataset->training_set[0]->items[7], "09");
    
    assert_equal_string(dataset->validation_set[dataset->count - 1]->items[0], "08");
    assert_equal_string(dataset->validation_set[dataset->count - 1]->items[1], "09");
    assert_equal_string(dataset->validation_set[dataset->count - 1]->items[2], "10");
    assert_equal_string(dataset->training_set[dataset->count - 1]->items[0], "00");
    assert_equal_string(dataset->training_set[dataset->count - 1]->items[1], "01");
    assert_equal_string(dataset->training_set[dataset->count - 1]->items[2], "02");
    assert_equal_string(dataset->training_set[dataset->count - 1]->items[3], "03");
    assert_equal_string(dataset->training_set[dataset->count - 1]->items[4], "04");
    assert_equal_string(dataset->training_set[dataset->count - 1]->items[5], "05");
    assert_equal_string(dataset->training_set[dataset->count - 1]->items[6], "06");
    assert_equal_string(dataset->training_set[dataset->count - 1]->items[7], "07");
    
    free_input_data(raw_input_data);
    free_data_set(dataset);
    return 0;
}

int test_training_make_data_set(void) {
    int sets_count = 3;
    int train_data_count = 100;
    int validation_data_count = 20;
    data_set *dataset = make_data_set(sets_count, train_data_count, validation_data_count);
    
    assert_not_equal(dataset->training_set, NULL);
    assert_not_equal(dataset->validation_set, NULL);
    assert_equal(dataset->count, sets_count);
    assert_equal(dataset->training_set[0]->count, train_data_count);
    assert_equal(dataset->validation_set[0]->count, validation_data_count);
    
    free_data_set(dataset);
    
    return 0;
}

int test_training_free_data_set(void) {
    int sets_count = 3;
    int train_data_count = 100;
    int validation_data_count = 20;
    data_set *dataset = make_data_set(sets_count, train_data_count, validation_data_count);
    
    int result = free_data_set(dataset);
    
    assert_equal(result, 0);
    return 0;
}
