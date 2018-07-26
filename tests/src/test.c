//
//  test.c
//  netapix
//
//  Created by Pavel Kondrashkov on 5/22/18.
//  Copyright © 2018 Touchlane LLC. All rights reserved.
//

#include <stdio.h>
#include "unit_test.h"

#include "test_utils.h"
#include "test_math.h"
#include "test_connected.h"
#include "test_loss.h"
#include "test_training.h"
#include "test_run.h"
#include "test_validation.h"

int main() {
    test(test_utils_make_matrix, "utils test make matrix from a vector.");
    test(test_utils_make_tensor, "utils test make tensor from a vector.");
    test(test_utils_make_array_of_tensors, "utils test make array of tensor from a vector.");
    
    test(test_math_linear, "math test linear activation");
    test(test_math_derivative_linear, "math test linear activation derivative.");
    test(test_math_relu, "math test relu activation.");
    test(test_math_derivative_relu, "math test relu activation derivative.");
    test(test_math_th, "math test th activation.");
    test(test_math_derivative_th, "math test th activation derivative.");
    test(test_math_logistic, "math test logistic activation.");
    test(test_math_derivative_logistic, "math test logistic activation derivative.");
    test(test_math_softmax, "math test softmax activation function.");
    test(test_math_msqe, "math test msqe loss function.");
    test(test_math_cross_entropy, "math test cross entropy loss function.");
    test(test_math_sqroot, "math test square root function.");
    test(test_math_activate, "math test activate switch function.");
    test(test_math_derivative, "math test derivative switch function.");
    test(test_math_average, "math test average function.");
    test(test_math_min, "math test min function.");
    test(test_math_max, "math test max function.");
    
    test(test_connected_make_connected_layer, "connected test make connected layer.");
    test(test_connected_forward, "connected test connected forward.");
    test(test_connected_backward, "connected test connected backward.");
    test(test_connected_calc_corrections, "connected test connected calc corrections.");
    test(test_connected_free_layer_first_layer, "connected test free layer as the first layer.");
    test(test_connected_free_layer_no_corrections, "connected test free layer with no corrections set.");
    
    test(test_loss_make_loss_layer, "loss test make loss layer.");
    test(test_loss_forward, "loss test loss forward.");
    test(test_loss_backward_scalar_msqe, "loss test loss backward scalar msqe.");
    test(test_loss_backward_softmax_msqe, "loss test loss backward softmax msqe.");
    test(test_loss_backward_softmax_cross_entropy, "loss test loss backward softmax cross entropy.");
    
    test(test_run_make_run_root, "run test make run root.");
    test(test_run_free_run_root, "run test free run root.");
    test(test_run_make_run_weights, "run test make run weights.");
    test(test_run_free_run_weights, "run test free run weights.");
    test(test_run_make_network, "run test make run network.");
    test(test_run_free_network, "run test free run network.");
    test(test_run_free_layer, "run test free layer.");
    
    test(test_training_validation_data_size, "training test validation data size from input data.");
    test(test_training_validation_data_size_min, "training test validation data size from input data min value.");
    test(test_training_prepare_data_set, "training test prepare data set from input data.");
    test(test_training_make_data_set, "training test make data set network.");
    test(test_training_free_data_set, "training test free data set network.");
    
    test(test_validation_make_validation_root, "validation test make validation root.");
    test(test_validation_free_validation_root, "validation test free validation root.");
    test(test_validation_make_validation_network, "validation test make validation network.");
    test(test_validation_free_validation_network, "validation test free validation network.");
    test(test_validation_set_validation_offset, "validation test set validation offset.");
    

    printf("PASSED: %d\nFAILED: %d\n", test_passed, test_failed);
    return (test_failed > 0);
}
