//
//  Constants.h
//  Parser
//
//  Created by Evgeny Dedovets on 4/22/18.
//  Copyright © 2018 Touchlane LLC. All rights reserved.
//

#ifndef Constants_h
#define Constants_h

#define CONFIG_KEY "[config]"

#define BATCH_KEY "batch"
#define THREADS_KEY "threads"

#define INIT_KEY "init"
#define XAVIER_KEY "xavier"

#define LEARNING_KEY "learning"
#define GRADIENT_DESCENT_KEY "gradient"

#define REGULARIZATION_KEY "regularization"
#define L1_REGULARIZATION_KEY "L1"
#define L2_REGULARIZATION_KEY "L2"

#define ETA_KEY "eta"
#define CHANNELS_KEY "channels"
#define ACCURACY_KEY "accuracy"
#define VALIDATION_KEY "validation"
#define SAVE_FREQUENCY_KEY "backup"
#define LAMBDA_KEY "lambda"
#define MOMENTUM_KEY "momentum"
#define ALPHA_KEY "alpha"
#define BETA_KEY "beta"
#define GAMMA_KEY "gamma"

#define CONNECTED_LAYER_KEY "[connected]"
#define CONVOLUTIONAL_LAYER_KEY "[convolutional]"
#define INPUT_KEY "input"
#define STRIDE_KEY "stride"
#define PADDING_KEY "padding"
#define WIDTH_KEY "width"
#define HEIGHT_KEY "height"

#define ACTIVATION_KEY "activation"
#define LINEAR_KEY "linear"
#define RELU_KEY "relu"
#define LOGISTIC_KEY "logistic"
#define TH_KEY "th"
#define SOFTMAX_KEY "softmax"

#define LOSS_LAYER_KEY "[loss]"

#define MSQE_KEY "msqe"
#define CROSS_ENTROPY_KEY "entropy"

typedef enum {
    NPX_SUCCESS = 0,
    NPX_FAIL = -1
} npx_status;

typedef enum {
    ERROR_OPEN = -1,
    ERROR_FORMAT = -2
} npx_error;

#define DEFAULT_CHANNELS_VALUE -1
#define DEFAULT_BATCH_VALUE 1
#define DEFAULT_THREADS_VALUE 1
#define DEFAULT_ETA_VALUE -1
#define DEFAULT_ACCURACY_VALUE -1
#define DEFAULT_VALIDATION_VALUE -1
#define DEFAULT_GAMMA_VALUE 1
#define DEFAULT_ALPHA_VALUE -1
#define DEFAULT_BETA_VALUE -1
#define DEFAULT_MOMENTUM_VALUE 0
#define DEFAULT_LAMBDA_VALUE 0
#define DEFAULT_NETWORK_INPUT_SIZE_VALUE -1
#define DEFAULT_NETWORK_TARGET_SIZE_VALUE -1
#define DEFAULT_SIZE_VALUE -1
#define DEFAULT_FILTERS_COUNT_VALUE -1
#define DEFAULT_INPUT_SIZE_VALUE -1
#define DEFAULT_OUTPUT_SIZE_VALUE -1
#define DEFAULT_FILTER_SIZE_VALUE -1
#define DEFAULT_STRIDE_VALUE -1
#define DEFAULT_PADDING_VALUE -1

#define ERROR_OPEN_FILE_MSG "Unable to open file: %s.\n"
#define ERROR_FORMAT_NOT_VALID_BATCH_MSG "Batch size should be > 0, actual value: %d.\n"
#define ERROR_FORMAT_NOT_VALID_VALIDATION_MSG "Validation value should be > 0, actual value: %d.\n"
#define ERROR_FORMAT_NOT_VALID_SAVE_FREQUENCY_MSG "Save frequency value should be > 0, actual value: %d.\n"
#define ERROR_FORMAT_NOT_VALID_THREADS_MSG "Threads count should be > 0, actual value: %d.\n"
#define ERROR_FORMAT_BATCH_LESS_THEN_THREADS_MSG "Threads count should be less or equal to batch, current threads: %d, current batch: %d.\n"
#define ERROR_FORMAT_MISSING_LEARNING_TYPE_MSG "Learning type not specified.\n"
#define ERROR_FORMAT_MISSING_ETA_MSG "Learning rate not specified.\n"
#define ERROR_FORMAT_NOT_VALID_ETA_MSG "Learning rate should be > 0, actual value: %f.\n"
#define ERROR_FORMAT_MISSING_ALPHA_MSG "Alpha not specified.\n"
#define ERROR_FORMAT_NOT_VALID_ALPHA_MSG "Alpha should be > 0 & < 1, actual value: %f.\n"
#define ERROR_FORMAT_MISSING_BETA_MSG "Beta not specified.\n"
#define ERROR_FORMAT_NOT_VALID_BETA_MSG "Beta should be > 1, actual value: %f.\n"
#define ERROR_FORMAT_MISSING_GAMMA_MSG "Gamma not specified.\n"
#define ERROR_FORMAT_NOT_VALID_GAMMA_MSG "Gamma should be > 0, actual value: %f.\n"
#define ERROR_FORMAT_NOT_VALID_MOMENTUM_MSG "Momentum should be >= 0, actual value: %f.\n"
#define ERROR_FORMAT_NOT_VALID_LAMBDA_MSG "Lambda should be > 0, actual value: %f.\n"
#define ERROR_FORMAT_MISSING_ACCURACY_MSG "Accuracy not specified.\n"
#define ERROR_FORMAT_NOT_VALID_ACCURACY_MSG "Accuracy should be > 0, actual value: %f.\n"
#define ERROR_FORMAT_MISSING_INPUT_SIZE_MSG "Input not specified for %s layer at index %d.\n"
#define ERROR_FORMAT_NOT_VALID_INPUT_SIZE_MSG "Input should be > 0 for %s layer at index %d, actual value: %d.\n"
#define ERROR_FORMAT_MISSING_ACTIVATION_MSG "Valid activation not specified for %s layer at index %d.\n"
#define ERROR_FORMAT_MISSING_FILTER_WIDTH_MSG "Filter width not specified for %s layer at index %d.\n"
#define ERROR_FORMAT_MISSING_FILTER_HEIGHT_MSG "Filter height not specified for %s layer at index %d.\n"
#define ERROR_FORMAT_NOT_VALID_FILTER_WIDTH_MSG "Filter width should be > 0 for %s layer at index %d, actual value: %d.\n"
#define ERROR_FORMAT_NOT_VALID_FILTER_HEIGHT_MSG "Filter height should be > 0 for %s layer at index %d, actual value: %d.\n"
#define ERROR_FORMAT_MISSING_FILTERS_COUNT_MSG "Filter count not specified for %s layer at index %d.\n"
#define ERROR_FORMAT_NOT_VALID_FILTERS_COUNT_MSG "Filter count should be > 0 for %s layer at index %d, actual value: %d.\n"
#define ERROR_FORMAT_MISSING_CHANNELS_MSG "Channels required for networks starts with %s layer.\n"
#define ERROR_FORMAT_MISSING_INPUT_WIDTH_MSG "Input width not specified. Required for networks starts with %s layer.\n"
#define ERROR_FORMAT_MISSING_INPUT_HEIGHT_MSG "Input height not specified. Required for networks starts with %s layer.\n"
#define ERROR_FORMAT_NOT_VALID_INPUT_WIDTH_MSG "Input width value shoule be > 0, actual value: %d.\n"
#define ERROR_FORMAT_NOT_VALID_INPUT_HEIGHT_MSG "Input height value shoule be > 0, actual value: %d.\n"
#define ERROR_FORMAT_NOT_VALID_CHANNELS_MSG "Channels value shoule be > 0, actual value: %d.\n"
#define ERROR_FORMAT_MISSING_STRIDE_MSG "Stride not specified for %s layer at index %d.\n"
#define ERROR_FORMAT_NOT_VALID_STRIDE_MSG "Stride should be > 0 for %s layer at index %d, actual value: %d.\n"
#define ERROR_FORMAT_MISSING_PADDING_MSG "Padding not specified for %s layer at index %d.\n"
#define ERROR_FORMAT_NOT_VALID_PADDING_MSG "Padding should be >= 0 for %s layer at index %d, actual value: %d.\n"
#define ERROR_FORMAT_LOSS_POSITION_MSG "%s should be at the very last position.\n"
#define ERROR_FORMAT_LAYERS_COUNT_MSG "At least one data layer should exist in network.\n"
#define ERROR_FORMAT_LAYER_TYPE_IS_NOT_CORRECT_MSG "Layer at position %d has not allowed type.\n"
#define ERROR_FORMAT_BINDING_LAYERS_MSG "Can't bind layers with positions %d and %d.\n"
#define ERROR_FORMAT_CONVOLUTIONAL_AFTER_CONNECTED_MSG "[convolutional] layer can't follow [connected].\n"
#define ERROR_FORMAT_NO_LAYERS_MSG "No layers found in the npx.\n"
#define ERROR_FORMAT_MISSING_INIT_TYPE_MSG "Missing weights initialization type.\n"
#define ERROR_FORMAT_SOFTMAX_NOT_ALLOWED_MSG "Softmax not allowed for any layer except one preventing loss.\n"
#define ERROR_FORMAT_SOFTMAX_REQUIRED_MSG "Softmax required for last computiona layer while using CROSS ENTROPY loss function.\n"

#define ERROR_NOT_ENOUGH_DATA_MSG "Training set contains not enough items. Number of items should be more or equal to the batch size. Actual number of items: %d, actual batch size: %d.\n"

#define FATAL_ERROR_ACTIVATION_FAIL_MSG "Ooops, something went wrong. Can't apply activation. Report to developer.\n"
#define FATAL_ERROR_LOSS_FAIL_MSG "Ooops, something went wrong. Can't apply loss. Report to developer.\n"
#define FATAL_ERROR_NDF_LEARNING_TYPE_FAIL_MSG "Ooops, something went wrong. Not defined learning type found. Report to developer.\n"
#define FATAL_ERROR_INIT_CONNECTED_WEIGHTS_FAIL_MSG "Ooops, something went wrong. Attempt to init connected weights with not defined initialization type. Report to developer.\n"
#define FATAL_ERROR_INIT_CONVOLUTIONAL_WEIGHTS_FAIL_MSG "Ooops, something went wrong. Attempt to init convolutional weights with not defined initialization type. Report to developer.\n"
#define FATAL_ERROR_MAKE_WEIGHTS_FAIL_MSG "Ooops, something went wrong. Attempt to make weights for unexpected layer type. Report to developer.\n"
#define FATAL_ERROR_INIT_WEIGHTS_FAIL_MSG "Ooops, something went wrong. Attempt to init %s weights. Report to developer.\n"
#define FATAL_ERROR_MAKE_BATCH_CORRECTIONS_FAIL_MSG "Ooops, something went wrong. Attempt to make batch corrections for unexpected layer type %s. Report to developer.\n"

#define FATAL_ERROR_FREE_BATCH_CORRECTIONS_FAIL_MSG "Ooops, something went wrong. Attempt to free batch corrections for unexpected layer type %s. Report to developer.\n"

#define FATAL_ERROR_FREE_WEIGHTS_FAIL_MSG "Ooops, something went wrong. Attempt to free weights for unexpected layer type %s. Report to developer.\n"
#define FATAL_ERROR_MAKE_LAYER_FAIL_MSG "Ooops, something went wrong. Attempt to make unexpected layer. Report to developer.\n"
#define FATAL_ERROR_ACCESS_NDF_LAYER_FAIL_MSG "Ooops, something went wrong. Attempt to acsess [NOT DEFINED LAYER]. Report to developer.\n"
#define FATAL_ERROR_MAKE_NETWORK_FAIL_MSG "Ooops, something went wrong. Attempt to make network failed. Unexpected layers order. Report to developer.\n"

#define FATAL_ERROR_NPT_INPUT_FORMAT_FAIL_MSG "Input format not correct. Expected input length: %d, actual value: %d at file: %s\n"
#define FATAL_ERROR_NPT_TARGET_FORMAT_FAIL_MSG "Target format not correct. Expected target length: %d, actual value: %d at file: %s\n"

#define FATAL_ERROR_NPW_FORMAT_FAIL_MSG "Weight file format is not correct. Undefined weights count at file: %s.\n"
#define FATAL_ERROR_NPW_VALUES_FORMAT_FAIL_MSG "Weight values format is not correct. Expected values count: %d, actual: %d at file: %s\n"
#define FATAL_ERROR_NPW_CONFIG_FORMAT_FAIL_MSG "Layer config type is not correct. Error reading at file: %s\n"
#define FATAL_ERROR_WRITE_LOSS_LAYER_NPO_FAIL_MSG "Ooops, something went wrong. Attempt to write loss layer output. Report to developer.\n"

#define FATAL_ERROR_NPI_VALUES_FORMAT_FAIL_MSG "Input values format is not correct. Expected values count: %d, actual: %d at file: %s\n"

#endif /* Constants_h */
