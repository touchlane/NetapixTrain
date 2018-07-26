//
//  config.c
//  netapix
//
//  Created by Evgeny Dedovets on 4/20/18.
//  Copyright © 2018 Touchlane LLC. All rights reserved.
//

#include "config.h"
#include <string.h>
#include "utils.h"
#include "constants.h"
#include <stdlib.h>
#include <stdio.h>

typedef enum {
    PARSING_CONFIG,
    PARSING_LAYER,
    NDEF_PARSING
} parsing_level;

int pick_convolutional_output_size(int input_size, int filter_size, int padding, int stride) {
    int size = 1;
    while (size * stride - stride <= input_size - filter_size + 2 * padding) {
        if (size * stride - stride == input_size - filter_size + 2 * padding) {
            return size;
        } else {
            size = size + 1;
        }
    }
    return -1;
}

int init_inp_out(npx_config *npx) {
    int convolutional_allowed = 1;
    switch (npx->net[0].type) {
        case CONNECTED:
            convolutional_allowed = 0;
            npx->settings->input_length = npx->net[0].input_length;
            break;
        case CONVOLUTIONAL:
            npx->net[0].input_depth = npx->settings->channels;
            npx->net[0].input_height = npx->settings->height;
            npx->net[0].input_width = npx->settings->width;
            npx->settings->input_length = npx->settings->channels * npx->settings->width * npx->settings->height;
            break;
        default:
            printf(ERROR_FORMAT_LAYER_TYPE_IS_NOT_CORRECT_MSG, 0);
            return ERROR_FORMAT;
            break;
    }
    for (int i = 0; i < npx->size; i++) {
        switch (npx->net[i].type) {
            case CONNECTED:
                convolutional_allowed = 0;
                npx->net[i].output_length = npx->net[i + 1].input_length;
                break;
            case CONVOLUTIONAL:
                if (convolutional_allowed > 0) {
                    if (i > 0) {
                        npx->net[i].input_depth = npx->net[i - 1].channels;
                        npx->net[i].input_height = npx->net[i - 1].output_height;
                        npx->net[i].input_width = npx->net[i - 1].output_width;
                    }
                    npx->net[i].output_width = pick_convolutional_output_size(npx->net[i].input_width, npx->net[i].width, npx->net[i].padding, npx->net[i].stride);
                    npx->net[i].output_height = pick_convolutional_output_size(npx->net[i].input_height, npx->net[i].height, npx->net[i].padding, npx->net[i].stride);
                    if (npx->net[i].output_width < 0 || npx->net[i].output_height < 0) {
                        printf(ERROR_FORMAT_BINDING_LAYERS_MSG, i - 1, i);
                        return ERROR_FORMAT;
                    }
                    npx->net[i].output_length = npx->net[i].output_height * npx->net[i].output_width * npx->net[i].channels;
                    if (npx->net[i + 1].type == CONNECTED && npx->net[i + 1].input_length != npx->net[i].output_length) {
                        printf(ERROR_FORMAT_BINDING_LAYERS_MSG, i, i + 1);
                        return ERROR_FORMAT;
                    }
                } else {
                    printf(ERROR_FORMAT_CONVOLUTIONAL_AFTER_CONNECTED_MSG);
                    return ERROR_FORMAT;
                }
                break;
            case LOSS:
                if (npx->net[i - 1].output_length != npx->net[i].input_length) {
                    printf(ERROR_FORMAT_BINDING_LAYERS_MSG, i - 1, i);
                    return ERROR_FORMAT;
                } else {
                    npx->settings->target_length = npx->net[i].input_length;
                }
                break;
            case NDEF_LAYER:
                printf(ERROR_FORMAT_LAYER_TYPE_IS_NOT_CORRECT_MSG, 0);
                return ERROR_FORMAT;
                break;
        }
    }
    return 0;
}

int validate_npx(npx_config *npx) {
    if (validate_settings(npx->settings) < 0) {
        return ERROR_FORMAT;
    }
    if (npx->size > 0) {
        if (npx->net[0].type == CONVOLUTIONAL) {
            if (npx->settings->channels == DEFAULT_CHANNELS_VALUE) {
                printf(ERROR_FORMAT_MISSING_CHANNELS_MSG, CONVOLUTIONAL_LAYER_KEY);
                return ERROR_FORMAT;
            } else if (npx->settings->channels < 1) {
                printf(ERROR_FORMAT_NOT_VALID_CHANNELS_MSG, npx->settings->channels);
                return ERROR_FORMAT;
            }
            if (npx->settings->width == DEFAULT_SIZE_VALUE) {
                printf(ERROR_FORMAT_MISSING_INPUT_WIDTH_MSG, CONVOLUTIONAL_LAYER_KEY);
                return ERROR_FORMAT;
            } else if (npx->settings->width < 1) {
                printf(ERROR_FORMAT_NOT_VALID_INPUT_WIDTH_MSG, npx->settings->width);
                return ERROR_FORMAT;
            }
            if (npx->settings->height == DEFAULT_SIZE_VALUE) {
                printf(ERROR_FORMAT_MISSING_INPUT_HEIGHT_MSG, CONVOLUTIONAL_LAYER_KEY);
                return ERROR_FORMAT;
            } else if (npx->settings->height < 1) {
                printf(ERROR_FORMAT_NOT_VALID_INPUT_HEIGHT_MSG, npx->settings->height);
                return ERROR_FORMAT;
            }
        }
        if (npx->size < 2) {
            printf(ERROR_FORMAT_LAYERS_COUNT_MSG);
            return ERROR_FORMAT;
        }
        for(int i = 0; i < npx->size; i++) {
            if (validate_layer(npx->net[i], i, npx->size) < 0) {
                return ERROR_FORMAT;
            }
        }
        if (npx->net[npx->size - 1].loss == CROSS_ENTROPY && npx->net[npx->size - 2].activation != SOFTMAX) {
            printf(ERROR_FORMAT_SOFTMAX_REQUIRED_MSG);
            return ERROR_FORMAT;
        }
    } else {
        printf(ERROR_FORMAT_NO_LAYERS_MSG);
        return ERROR_FORMAT;
    }
    if (init_inp_out(npx) < 0) {
        return ERROR_FORMAT;
    }
    return 0;
}

int validate_layer(layer_config layer, int index, int count) {
    int is_valid = 1;
    if (layer.activation == SOFTMAX) {
        if (index != count - 2) {
            is_valid = 0;
            printf(ERROR_FORMAT_SOFTMAX_NOT_ALLOWED_MSG);
        }
    }
    switch (layer.type) {
        case CONNECTED:
            if (layer.input_length == DEFAULT_INPUT_SIZE_VALUE) {
                is_valid = 0;
                printf(ERROR_FORMAT_MISSING_INPUT_SIZE_MSG, CONNECTED_LAYER_KEY, index);
            } else if (layer.input_length < 1) {
                is_valid = 0;
                printf(ERROR_FORMAT_NOT_VALID_INPUT_SIZE_MSG, CONNECTED_LAYER_KEY, index, layer.input_length);
            }
            if (layer.activation == NDEF_ACTIVATION) {
                is_valid = 0;
                printf(ERROR_FORMAT_MISSING_ACTIVATION_MSG, CONNECTED_LAYER_KEY, index);
            }
            break;
        case LOSS:
            if (index != count - 1) {
                is_valid = 0;
                printf(ERROR_FORMAT_LOSS_POSITION_MSG, LOSS_LAYER_KEY);
            }
            if (layer.input_length == DEFAULT_INPUT_SIZE_VALUE) {
                is_valid = 0;
                printf(ERROR_FORMAT_MISSING_INPUT_SIZE_MSG, LOSS_LAYER_KEY, index);
            } else if (layer.input_length < 1) {
                is_valid = 0;
                printf(ERROR_FORMAT_NOT_VALID_INPUT_SIZE_MSG, LOSS_LAYER_KEY, index, layer.input_length);
            }
            if (layer.loss == NDEF_LOSS) {
                is_valid = 0;
                printf(ERROR_FORMAT_MISSING_ACTIVATION_MSG, CONNECTED_LAYER_KEY, index);
            }
            break;
        case CONVOLUTIONAL:
            if (layer.activation == NDEF_ACTIVATION) {
                is_valid = 0;
                printf(ERROR_FORMAT_MISSING_ACTIVATION_MSG, CONVOLUTIONAL_LAYER_KEY, index);
            }
            if (layer.width == DEFAULT_FILTER_SIZE_VALUE) {
                is_valid = 0;
                printf(ERROR_FORMAT_MISSING_FILTER_WIDTH_MSG, CONVOLUTIONAL_LAYER_KEY, index);
            } else if (layer.width < 1) {
                is_valid = 0;
                printf(ERROR_FORMAT_NOT_VALID_FILTER_WIDTH_MSG, CONVOLUTIONAL_LAYER_KEY, index, layer.width);
            }
            if (layer.height == DEFAULT_FILTER_SIZE_VALUE) {
                is_valid = 0;
                printf(ERROR_FORMAT_MISSING_FILTER_HEIGHT_MSG, CONVOLUTIONAL_LAYER_KEY, index);
            } else if (layer.height < 1) {
                is_valid = 0;
                printf(ERROR_FORMAT_NOT_VALID_FILTER_HEIGHT_MSG, CONVOLUTIONAL_LAYER_KEY, index, layer.height);
            }
            if (layer.channels == DEFAULT_FILTERS_COUNT_VALUE) {
                is_valid = 0;
                printf(ERROR_FORMAT_MISSING_FILTERS_COUNT_MSG, CONVOLUTIONAL_LAYER_KEY, index);
            } else if (layer.channels < 1) {
                is_valid = 0;
                printf(ERROR_FORMAT_NOT_VALID_FILTERS_COUNT_MSG, CONVOLUTIONAL_LAYER_KEY, index, layer.channels);
            }
            if (layer.padding == DEFAULT_PADDING_VALUE) {
                is_valid = 0;
                printf(ERROR_FORMAT_MISSING_PADDING_MSG, CONVOLUTIONAL_LAYER_KEY, index);
            } else if (layer.padding < 0) {
                is_valid = 0;
                printf(ERROR_FORMAT_NOT_VALID_PADDING_MSG, CONVOLUTIONAL_LAYER_KEY, index, layer.padding);
            }
            if (layer.stride == DEFAULT_STRIDE_VALUE) {
                is_valid = 0;
                printf(ERROR_FORMAT_MISSING_STRIDE_MSG, CONVOLUTIONAL_LAYER_KEY, index);
            } else if (layer.stride < 1) {
                is_valid = 0;
                printf(ERROR_FORMAT_NOT_VALID_STRIDE_MSG, CONVOLUTIONAL_LAYER_KEY, index, layer.stride);
            }
            break;
        case NDEF_LAYER:
            is_valid = 0;
            break;
    }
    if (is_valid == 0) {
        return ERROR_FORMAT;
    }
    return 0;
}

int validate_settings(settings_config *config) {
    int is_valid = 1;
    if (config->batch < 1) {
        is_valid = 0;
        printf(ERROR_FORMAT_NOT_VALID_BATCH_MSG, config->batch);
    }
    if (config->validation < 1) {
        is_valid = 0;
        printf(ERROR_FORMAT_NOT_VALID_VALIDATION_MSG, config->validation);
    }
    if (config->save_frequency < 1) {
        is_valid = 0;
        printf(ERROR_FORMAT_NOT_VALID_SAVE_FREQUENCY_MSG, config->save_frequency);
    }
    if (config->threads < 1) {
        is_valid = 0;
        printf(ERROR_FORMAT_NOT_VALID_THREADS_MSG, config->batch);
    }
    if (config->threads > config->batch) {
        is_valid = 0;
        printf(ERROR_FORMAT_BATCH_LESS_THEN_THREADS_MSG, config->threads, config->batch);
    }
    if (config->accuracy == DEFAULT_ACCURACY_VALUE) {
        is_valid = 0;
        printf(ERROR_FORMAT_MISSING_ACCURACY_MSG);
    } else if (config->accuracy <= 0) {
        is_valid = 0;
        printf(ERROR_FORMAT_NOT_VALID_ACCURACY_MSG, config->accuracy);
    }
    switch (config->init) {
        case NDEF_INITIALIZATION:
            printf(ERROR_FORMAT_MISSING_INIT_TYPE_MSG);
            is_valid = 0;
            break;
        default:
            break;
    }
    switch (config->learning) {
        case GRADIENT_DESCENT:
            if (config->eta == DEFAULT_ETA_VALUE) {
                is_valid = 0;
                printf(ERROR_FORMAT_MISSING_ETA_MSG);
            } else if (config->eta <= 0) {
                is_valid = 0;
                printf(ERROR_FORMAT_NOT_VALID_ETA_MSG, config->eta);
            }
            if (config->alpha == DEFAULT_ALPHA_VALUE) {
                is_valid = 0;
                printf(ERROR_FORMAT_MISSING_ALPHA_MSG);
            } else if (config->alpha > 1 || config->alpha <= 0) {
                is_valid = 0;
                printf(ERROR_FORMAT_NOT_VALID_ALPHA_MSG, config->alpha);
            }
            if (config->beta == DEFAULT_BETA_VALUE) {
                is_valid = 0;
                printf(ERROR_FORMAT_MISSING_BETA_MSG);
            } else if (config->beta <= 1) {
                is_valid = 0;
                printf(ERROR_FORMAT_NOT_VALID_BETA_MSG, config->beta);
            }
            if (config->gamma == DEFAULT_GAMMA_VALUE) {
                is_valid = 0;
                printf(ERROR_FORMAT_MISSING_GAMMA_MSG);
            } else if (config->gamma <= 0) {
                is_valid = 0;
                printf(ERROR_FORMAT_NOT_VALID_GAMMA_MSG, config->gamma);
            }
            if (config->momentum < 0) {
                is_valid = 0;
                printf(ERROR_FORMAT_NOT_VALID_MOMENTUM_MSG, config->momentum);
            }
            break;
        case NDEF_LEARN:
            is_valid = 0;
            printf(ERROR_FORMAT_MISSING_LEARNING_TYPE_MSG);
            break;
    }
    switch (config->regularization) {
        case L1:
            if (config->lambda < 0) {
                is_valid = 0;
                printf(ERROR_FORMAT_NOT_VALID_LAMBDA_MSG, config->lambda);
            }
            break;
        case L2:
            if (config->lambda < 0) {
                is_valid = 0;
                printf(ERROR_FORMAT_NOT_VALID_LAMBDA_MSG, config->lambda);
            }
            break;
        case NDEF_REGULARIZATION:
            break;
    }
    if (is_valid == 0) {
        return ERROR_FORMAT;
    }
    return 0;
}

npx_config *read_npx(const char *filename) {
    npx_config *npx = make_npx_config();
    char *buffer = NULL;
    unsigned long length = 0;
    int read_res = read_txt(filename, &buffer, &length);
    if (read_res < 0) {
        return npx;
    }
    int layers_count = 0;
    npx_option *head = parse_npx(buffer, length, &layers_count);
    npx->net = malloc(layers_count * sizeof(layer_config));
    npx->size = layers_count;
    int l_index = 0;
    parsing_level level = NDEF_PARSING;
    npx_option *curr_opt = head;
    layer_config *layer = NULL;
    while (curr_opt != NULL) {
        if (strcmp(curr_opt->key, CONFIG_KEY) == 0) {
            level = PARSING_CONFIG;
        } else if (is_layer(curr_opt->key) > 0) {
            level = PARSING_LAYER;
            npx->net[l_index] = make_layer_config();
            layer = &npx->net[l_index];
            layer->type = detect_layer_type(curr_opt->key);
            l_index++;
        } else {
            switch (level) {
                case PARSING_CONFIG:
                    if (strcmp(curr_opt->key, BATCH_KEY) == 0) {
                        sscanf(curr_opt->val, "%d", &npx->settings->batch);
                    } else if (strcmp(curr_opt->key, THREADS_KEY) == 0) {
                        sscanf(curr_opt->val, "%d", &npx->settings->threads);
                    } else if (strcmp(curr_opt->key, LEARNING_KEY) == 0) {
                        npx->settings->learning = detect_learning_type(curr_opt->val);
                    } else if (strcmp(curr_opt->key, REGULARIZATION_KEY) == 0) {
                        npx->settings->regularization = detect_regularization_type(curr_opt->val);
                    } else if (strcmp(curr_opt->key, ETA_KEY) == 0) {
                        sscanf(curr_opt->val, "%f", &npx->settings->eta);
                    } else if (strcmp(curr_opt->key, ACCURACY_KEY) == 0) {
                        sscanf(curr_opt->val, "%f", &npx->settings->accuracy);
                    } else if (strcmp(curr_opt->key, VALIDATION_KEY) == 0) {
                        sscanf(curr_opt->val, "%d", &npx->settings->validation);
                    } else if (strcmp(curr_opt->key, SAVE_FREQUENCY_KEY) == 0) {
                        sscanf(curr_opt->val, "%d", &npx->settings->save_frequency);
                    } else if (strcmp(curr_opt->key, LAMBDA_KEY) == 0) {
                        sscanf(curr_opt->val, "%f", &npx->settings->lambda);
                    } else if (strcmp(curr_opt->key, MOMENTUM_KEY) == 0) {
                        sscanf(curr_opt->val, "%f", &npx->settings->momentum);
                    } else if (strcmp(curr_opt->key, ALPHA_KEY) == 0) {
                        sscanf(curr_opt->val, "%f", &npx->settings->alpha);
                    } else if (strcmp(curr_opt->key, BETA_KEY) == 0) {
                        sscanf(curr_opt->val, "%f", &npx->settings->beta);
                    } else if (strcmp(curr_opt->key, GAMMA_KEY) == 0) {
                        sscanf(curr_opt->val, "%f", &npx->settings->gamma);
                    } else if (strcmp(curr_opt->key, CHANNELS_KEY) == 0) {
                        sscanf(curr_opt->val, "%d", &npx->settings->channels);
                    } else if (strcmp(curr_opt->key, INIT_KEY) == 0) {
                        npx->settings->init = detect_init_type(curr_opt->val);
                    } else if (strcmp(curr_opt->key, WIDTH_KEY) == 0) {
                        sscanf(curr_opt->val, "%d", &npx->settings->width);
                    } else if (strcmp(curr_opt->key, HEIGHT_KEY) == 0) {
                        sscanf(curr_opt->val, "%d", &npx->settings->height);
                    }
                    break;
                case PARSING_LAYER:
                    if (strcmp(curr_opt->key, INPUT_KEY) == 0) {
                        sscanf(curr_opt->val, "%d", &layer->input_length);
                    } else if (strcmp(curr_opt->key, ACTIVATION_KEY) == 0) {
                        if (layer->type == LOSS) {
                            layer->loss = detect_loss_type(curr_opt->val);
                        } else {
                            layer->activation = detect_activation_type(curr_opt->val);
                        }
                    } else if (strcmp(curr_opt->key, STRIDE_KEY) == 0) {
                        sscanf(curr_opt->val, "%d", &layer->stride);
                    } else if (strcmp(curr_opt->key, CHANNELS_KEY) == 0) {
                        sscanf(curr_opt->val, "%d", &layer->channels);
                    } else if (strcmp(curr_opt->key, WIDTH_KEY) == 0) {
                        sscanf(curr_opt->val, "%d", &layer->width);
                    } else if (strcmp(curr_opt->key, HEIGHT_KEY) == 0) {
                        sscanf(curr_opt->val, "%d", &layer->height);
                    } else if (strcmp(curr_opt->key, PADDING_KEY) == 0) {
                        sscanf(curr_opt->val, "%d", &layer->padding);
                    }
                    break;
                case NDEF_PARSING:
                    break;
            }
        }
        curr_opt = curr_opt->next;
    }
    free(buffer);
    free_npx_option(head);
    return npx;
}

npx_config *make_npx_config(void) {
    npx_config *npx = malloc(sizeof(*npx));
    npx->net = NULL;
    npx->size = 0;
    npx->settings = make_settings_config();
    return npx;
}

int free_npx_config(npx_config *npx) {
    free_settings_config(npx->settings);
    free(npx->net);
    free(npx);
    return 0;
}

settings_config *make_settings_config() {
    settings_config *settings = malloc(sizeof(*settings));
    settings->batch = DEFAULT_BATCH_VALUE;
    settings->threads = DEFAULT_THREADS_VALUE;
    settings->eta = DEFAULT_ETA_VALUE;
    settings->accuracy = DEFAULT_ACCURACY_VALUE;
    settings->validation = DEFAULT_VALIDATION_VALUE;
    settings->gamma = DEFAULT_GAMMA_VALUE;
    settings->alpha = DEFAULT_ALPHA_VALUE;
    settings->beta = DEFAULT_BETA_VALUE;
    settings->learning = NDEF_LEARN;
    settings->momentum = DEFAULT_MOMENTUM_VALUE;
    settings->regularization = NDEF_REGULARIZATION;
    settings->lambda = DEFAULT_LAMBDA_VALUE;
    settings->channels = DEFAULT_CHANNELS_VALUE;
    settings->init = NDEF_INITIALIZATION;
    settings->input_length = DEFAULT_NETWORK_INPUT_SIZE_VALUE;
    settings->target_length = DEFAULT_NETWORK_TARGET_SIZE_VALUE;
    settings->width = DEFAULT_SIZE_VALUE;
    settings->height = DEFAULT_SIZE_VALUE;
    return settings;
}

int free_settings_config(settings_config *set) {
    free(set);
    return 0;
}

layer_config make_layer_config() {
    layer_config layer;
    layer.activation = NDEF_ACTIVATION;
    layer.channels = DEFAULT_FILTERS_COUNT_VALUE;
    layer.output_length = DEFAULT_OUTPUT_SIZE_VALUE;
    layer.input_length = DEFAULT_INPUT_SIZE_VALUE;
    layer.loss = NDEF_LOSS;
    layer.width = DEFAULT_FILTER_SIZE_VALUE;
    layer.stride = DEFAULT_STRIDE_VALUE;
    layer.type = NDEF_LAYER;
    layer.width = DEFAULT_SIZE_VALUE;
    layer.height = DEFAULT_SIZE_VALUE;
    layer.padding = DEFAULT_PADDING_VALUE;
    layer.input_depth = DEFAULT_SIZE_VALUE;
    layer.input_width = DEFAULT_SIZE_VALUE;
    layer.input_height = DEFAULT_SIZE_VALUE;
    layer.output_width = DEFAULT_SIZE_VALUE;
    layer.output_height = DEFAULT_SIZE_VALUE;
    return layer;
}

npx_option *make_npx_option(void) {
    npx_option *opt = malloc(sizeof(*opt));
    opt->key = NULL;
    opt->val = NULL;
    opt->next = NULL;
    return opt;
}

int free_npx_option(npx_option* opt) {
    npx_option *curr = NULL;
    npx_option *next = opt;
    while (next != NULL) {
        curr = next;
        next = curr->next;
        free(curr->key);
        free(curr->val);
        free(curr);
    }
    return 0;
}

weights_init_type detect_init_type(const char* val) {
    if (strcmp(val, XAVIER_KEY) == 0) {
        return XAVIER;
    }
    return NDEF_INITIALIZATION;
}

loss_type detect_loss_type(const char* val) {
    if (strcmp(val, MSQE_KEY) == 0) {
        return MSQE;
    } else if (strcmp(val, CROSS_ENTROPY_KEY) == 0) {
        return CROSS_ENTROPY;
    }
    return NDEF_LOSS;
}

int is_layer(const char* val) {
    if ((strcmp(val, CONNECTED_LAYER_KEY) == 0)
        || (strcmp(val, LOSS_LAYER_KEY) == 0) || (strcmp(val, CONVOLUTIONAL_LAYER_KEY) == 0)) {
        return 1;
    }
    return 0;
}

activation_type detect_activation_type(const char* val) {
    if (strcmp(val, LINEAR_KEY) == 0) {
        return LINEAR;
    } else if (strcmp(val, RELU_KEY) == 0) {
        return RELU;
    } else if (strcmp(val, LOGISTIC_KEY) == 0) {
        return LOGISTIC;
    } else if (strcmp(val, TH_KEY) == 0) {
        return TH;
    } else if (strcmp(val, SOFTMAX_KEY) == 0) {
        return SOFTMAX;
    }
    return NDEF_ACTIVATION;
}

learn_type detect_learning_type(const char* val) {
    if (strcmp(val, GRADIENT_DESCENT_KEY) == 0) {
        return GRADIENT_DESCENT;
    }
    return NDEF_LEARN;
}

layer_type detect_layer_type(const char* val) {
    if (strcmp(val, LOSS_LAYER_KEY) == 0) {
        return LOSS;
    } else if (strcmp(val, CONNECTED_LAYER_KEY) == 0) {
        return CONNECTED;
    } else if (strcmp(val, CONVOLUTIONAL_LAYER_KEY) == 0) {
        return CONVOLUTIONAL;
    }
    return NDEF_LAYER;
}

regularization_type detect_regularization_type(const char* val) {
    if (strcmp(val, L1_REGULARIZATION_KEY) == 0) {
        return L1;
    } else if (strcmp(val, L2_REGULARIZATION_KEY) == 0) {
        return L2;
    }
    return NDEF_REGULARIZATION;
}

npx_option *parse_npx(const char *buffer, unsigned long length, int *layers_count) {
    int count = 0;
    npx_option *head = make_npx_option();
    npx_option *curr_opt = head;
    if (length > 0) {
        curr_opt->key = malloc(length*sizeof(char));
        curr_opt->val = malloc(length*sizeof(char));
    }
    char *line = curr_opt->key;
    int offset = 0;
    for (int i = 0; i < length; i++) {
        char c = buffer[i];
        if(c != ' ' && c != '\t' && c != '\n') {
            if (c == '=') {
                line[offset] = '\0';
                line = curr_opt->val;
                offset = 0;
            } else {
                line[offset] = c;
                offset++;
            }
        } else if (c == '\n'){
            line[offset] = '\0';
            offset = 0;
            if (length - i > 1) {
                if(is_layer(curr_opt->key)) {
                    count++;
                }
                curr_opt->next = make_npx_option();
                curr_opt = curr_opt->next;
                curr_opt->key = malloc(length*sizeof(char));
                curr_opt->val = malloc(length*sizeof(char));
                line = curr_opt->key;
            }
        }
    }
    if (length > 0) {
        line[offset] = '\0';
    }
    *layers_count = count;
    return head;
}
