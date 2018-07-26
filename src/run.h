//
//  run.h
//  netapix
//
//  Created by Pavel Kondrashkov on 6/5/18.
//  Copyright © 2018 Touchlane LLC. All rights reserved.
//

#ifndef run_h
#define run_h

#include <stdio.h>
#include "config.h"

typedef enum {
    WEIGHT_TRAIN_TYPE,
    WEIGHT_RUN_TYPE
} weight_type;

typedef struct {
    void **layers;
    int count;
} run_network;

typedef struct {
    weight_type type;
    float *values;
    int count;
} run_weights;

typedef struct {
    run_weights *weights;
    run_network network;
    float *input_data;
} run_root;

run_network make_run_network(layer_config *configs, int layers_count, run_weights *weights, float *input_data);
int free_run_root(run_root *root);
run_weights make_run_weights(layer_config config);
int free_run_weights(run_weights weights);

run_root *make_run_root(layer_config *configs, int layers_count, float *input_data);
int free_run_network(run_network net);

int run(char *npi_path, char *weights_path, char *output_path);

int forward(void *layer);
int read_npi(char *path, float **input, int *input_size);
int write_npo(char *path, float *output, int output_length);
int write_layer_output(char *path, void *layer);
int free_layer(void *layer, int is_first_layer);

int read_layer_configs_from_npw(char *path, layer_config **configs, int *weights_count);

#endif /* run_h */
