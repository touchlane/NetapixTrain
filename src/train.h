//
//  train.h
//  netapix
//
//  Created by Evgeny Dedovets on 4/28/18.
//  Copyright © 2018 Touchlane LLC. All rights reserved.
//

#ifndef train_h
#define train_h

#include "run.h"

typedef struct {
    float eta;
    float alpha;
    float beta;
    float momentum;
    float lambda;
    float gamma;
    regularization_type regularization;
    learn_type learning;
    /// Errors array size of batchSize. Each element belongs to separate loss layer.
    float *batch_errors;
    float prev_error;
    int batch;
    int iteration_number;
    int train_set_number;
} train_params;

typedef struct {
    char **items;
    int count;
} input_data;

typedef struct {
    input_data **validation_set;
    input_data **training_set;
    int count;
} data_set;

typedef struct {
    /// Reference to the 2D array of layers with height = batchSize, width = layersCount.
    void **layers;
    data_set *dataset;
    int position_in_batch;
    int *train_set_number;
    int *iteration_number;
    int batch;
    int input_length;
    int target_length;
    int count;
} train_network;

typedef struct {
    weight_type type;
    float *values;
    float *prev_delta;
    float **corrections;
    train_params *params;
    int count;
} train_weights;

typedef struct {
    /// Array of batch weights struct with size = weightsCount.
    train_weights *weights;
    train_network *networks;
    /// Pointer to a train params.
    train_params *params;
    npx_config *npx;
    data_set *data_set;
} train_root;

train_network make_train_network(npx_config *npx, data_set *data, train_weights *weights, int position_in_batch, int *iteration_number, int *train_set_number, float *error);
int clean_train_network(train_network net);

int init_weights(train_weights *weights, npx_config *npx);
int xavier_init_connected_weights(float *weights, layer_config config);
int xavier_init_convolutional_weights(float *weights, layer_config config);

int train(char *npx_path, char *train_path, char *weights_path, char *output_path);

void *compute_network(void *n);
void *update_weights(void *w);
float *input_from_layer(void *layer);
int backward(void *layer);
int calc_corrections(void *layer);
float reg_add(regularization_type type, float lambda, float multiplier);
int update_params(train_params *params, float error);

data_set *make_data_set(int sets_count, int train_data_count, int validation_data_count);
int free_data_set(data_set *set);
data_set *prepare_data_set(input_data *raw_input_data, int validation_data_count);

train_params *make_train_params(settings_config *settings);
int free_train_params(train_params *params);

train_root *make_train_root(npx_config *npx, data_set *data);
int free_train_root(train_root *root);

train_weights make_train_weights(layer_config config, train_params *params);
int clean_train_weights(train_weights weights);

int read_npt(char *path, float *input, float *target, int input_length, int target_length);
int shuffle(input_data *data);
int validate_data(input_data *data, int input_length, int target_length, int batch_size, int validation_count);
int number_of_items_at_path(char *path);
input_data *read_train_data(char *path);
input_data *make_input_data(int items_count);
int free_input_data(input_data *data);

int validation_size(int raw_input_data_size, int validation_percentage);

int compare_layer_config(layer_config l, layer_config r);
int write_npw(char *path, train_weights *weights, layer_config *configs, int weights_count);
int read_npw(char *path, void *weights, int weights_count);

loss_mode detect_loss_mode(layer_config config);

#endif /* train_h */
