//
//  validation.h
//  netapix
//
//  Created by Pavel Kondrashkov on 6/11/18.
//  Copyright © 2018 Touchlane LLC. All rights reserved.
//

#ifndef validation_h
#define validation_h

#include <stdio.h>
#include "run.h"
#include "train.h"

typedef struct {
    void **layers;
    int input_length;
    int target_length;
    int count;
    input_data *validation_data;
    int validation_offset;
    int validation_items_count;
    float *error;
    float **batch_errors_ref;
} validation_network;

typedef struct {
    validation_network *networks;
    int networks_count;
    data_set *dataset;
    npx_config *npx;
    float *batch_errors;
} validation_root;

float cross_validation(validation_root *root, int iteration);

validation_root *make_validation_root(npx_config *npx, data_set *dataset, train_weights *weights);
int free_validation_root(validation_root *root);

validation_network make_validation_network(npx_config *npx, train_weights *weights, float **batch_error_ref);
int free_validation_network(validation_network net);

void set_validation_offset(int validation_data_count, validation_network *networks, int networks_count);

#endif /* validation_h */
