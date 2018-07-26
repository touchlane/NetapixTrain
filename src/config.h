//
//  config.h
//  netapix
//
//  Created by Evgeny Dedovets on 4/20/18.
//  Copyright © 2018 Touchlane LLC. All rights reserved.
//

#ifndef config_h
#define config_h

typedef enum {
    CONVOLUTIONAL,
    CONNECTED,
    LOSS,
    NDEF_LAYER
} layer_type;

typedef enum {
    GRADIENT_DESCENT,
    NDEF_LEARN
} learn_type;

typedef enum {
    SOFTMAX,
    RELU,
    TH,
    LOGISTIC,
    LINEAR,
    NDEF_ACTIVATION
} activation_type;

typedef enum {
    MSQE,
    CROSS_ENTROPY,
    NDEF_LOSS
} loss_type;

typedef enum {
    SOFTMAX_MODE,
    SCALAR_DERIVATIVE_MODE,
    NDEF_LOSS_MODE
} loss_mode;

typedef enum {
    L1,
    L2,
    NDEF_REGULARIZATION
} regularization_type;

typedef enum {
    XAVIER,
    NDEF_INITIALIZATION
} weights_init_type;

typedef struct npx_option {
    char *key;
    char *val;
    struct npx_option *next;
} npx_option;

typedef struct {
    layer_type type;
    int input_width;
    int input_height;
    int input_depth;
    int output_width;
    int output_height;
    int input_length;
    int output_length;
    activation_type activation;
    int width;
    int height;
    int padding;
    int stride;
    int channels;
    loss_type loss;
} layer_config;

typedef struct {
    int batch;
    int channels;
    int width;
    int height;
    int threads;
    learn_type learning;
    regularization_type regularization;
    weights_init_type init;
    float eta;
    float accuracy;
    int validation;
    int save_frequency;
    float lambda;
    float momentum;
    float gamma;
    float beta;
    float alpha;
    int input_length;
    int target_length;
} settings_config;

typedef struct {
    layer_config *net;
    settings_config *settings;
    int size;
} npx_config;

int pick_convolutional_output_size(int input_size, int filter_size, int padding, int stride);
int init_inp_out(npx_config *npx);
npx_config *read_npx(const char *filename);
int validate_npx(npx_config *npx);
int validate_settings(settings_config *config);
int validate_layer(layer_config layer, int index, int count);
npx_option *parse_npx(const char *buffer, unsigned long length, int *layers_count);
learn_type detect_learning_type(const char* val);
weights_init_type detect_init_type(const char* val);
regularization_type detect_regularization_type(const char* val);
layer_type detect_layer_type(const char* val);
activation_type detect_activation_type(const char* val);
loss_type detect_loss_type(const char* val);
int is_layer(const char* val);
npx_config* make_npx_config(void);
int free_npx_config(npx_config *npx);
settings_config *make_settings_config(void);
int free_settings_config(settings_config *set);
layer_config make_layer_config(void);
npx_option *make_npx_option(void);
int free_npx_option(npx_option* opt);

#endif /* config_h */
