//
//  test_math.c
//  test
//
//  Created by Pavel Kondrashkov on 5/24/18.
//  Copyright © 2018 Touchlane LLC. All rights reserved.
//

#include "test_math.h"
#include "unit_test.h"
#include "math.h"
#include "config.h"

int test_math_linear(void) {
    float neuron1 = 1.2;
    float neuron2 = -1.2;
    float neuron3 = 0;
    
    neuron1 = linear(neuron1);
    neuron2 = linear(neuron2);
    neuron3 = linear(neuron3);
    
    assert_equal_float(neuron1, 1.2);
    assert_equal_float(neuron2, -1.2);
    assert_equal_float(neuron3, 0);
    
    return 0;
}

int test_math_derivative_linear(void) {
    float neuron1 = 1.2;
    float neuron2 = -1.2;
    float neuron3 = 0;
    
    neuron1 = derivative_linear(neuron1);
    neuron2 = derivative_linear(neuron2);
    neuron3 = derivative_linear(neuron3);
    
    assert_equal_float(neuron1, 1);
    assert_equal_float(neuron2, 1);
    assert_equal_float(neuron3, 1);
    
    return 0;
}

int test_math_relu(void) {
    float neuron1 = 1.2;
    float neuron2 = -1.2;
    float neuron3 = 0;
    
    neuron1 = relu(neuron1);
    neuron2 = relu(neuron2);
    neuron3 = relu(neuron3);
    
    assert_equal_float(neuron1, 1.2);
    assert_equal_float(neuron2, 0);
    assert_equal_float(neuron3, 0);
    
    return 0;
}

int test_math_derivative_relu(void) {
    float neuron1 = 1.2;
    float neuron2 = -1.2;
    float neuron3 = 0;
    
    neuron1 = derivative_relu(neuron1);
    neuron2 = derivative_relu(neuron2);
    neuron3 = derivative_relu(neuron3);
    
    assert_equal_float(neuron1, 1);
    assert_equal_float(neuron2, 0);
    assert_equal_float(neuron3, 0);
    
    return 0;
}

int test_math_th(void) {
    float neuron1 = 1.2;
    float neuron2 = -1.2;
    float neuron3 = -90;
    float neuron4 = 90;
    
    neuron1 = th(neuron1);
    neuron2 = th(neuron2);
    neuron3 = th(neuron3);
    neuron4 = th(neuron4);
    
    assert_equal_float(neuron1, 0.8336546);
    assert_equal_float(neuron2, -0.8336546);
    assert_equal_float(neuron3, -1);
    assert_equal_float(neuron4, 1);
    
    return 0;
}

int test_math_derivative_th(void) {
    float neuron1 = 1.2;
    float neuron2 = -1.2;
    float neuron3 = 0;
    float neuron4 = 20;
    
    neuron1 = derivative_th(neuron1);
    neuron2 = derivative_th(neuron2);
    neuron3 = derivative_th(neuron3);
    neuron4 = derivative_th(neuron4);
    
    assert_equal_float(neuron1, 0.30502);
    assert_equal_float(neuron2, 0.30502);
    assert_equal_float(neuron3, 1);
    assert_equal_float(neuron4, 0);
    
    return 0;
}

int test_math_logistic(void) {
    float neuron1 = 1.2;
    float neuron2 = -1.2;
    float neuron3 = -90;
    float neuron4 = 90;
    
    neuron1 = logistic(neuron1);
    neuron2 = logistic(neuron2);
    neuron3 = logistic(neuron3);
    neuron4 = logistic(neuron4);
    
    assert_equal_float(neuron1, 0.7685247);
    assert_equal_float(neuron2, 0.2314752);
    assert_equal_float(neuron3, 0);
    assert_equal_float(neuron4, 1);
    
    return 0;
}

int test_math_derivative_logistic(void) {
    float neuron1 = 1.2;
    float neuron2 = -1.2;
    float neuron3 = 0;
    
    neuron1 = derivative_logistic(neuron1);
    neuron2 = derivative_logistic(neuron2);
    neuron3 = derivative_logistic(neuron3);

    assert_equal_float(neuron1, 0.1778944);
    assert_equal_float(neuron2, 0.1778944);
    assert_equal_float(neuron3, 0.25);
    
    return 0;
}

int test_math_msqe(void) {
    float vector1[] = { 0, 0, 0, 1 };
    float vector2[] = { 1, 1, 1, 1 };
    float vector3[] = { 1, 1, 1, 0 };
    
    float target[] = { 0, 0, 0, 1 };
    int length = 4;
    
    float error1 = msqe(vector1, target, length);
    float error2 = msqe(vector2, target, length);
    float error3 = msqe(vector3, target, length);
    
    assert_equal_float(error1, 0);
    assert_equal_float(error2, 0.75);
    assert_equal_float(error3, 1);

    return 0;
}

int test_math_cross_entropy(void) {
    float vector1[] = { 0.1, 0.25, 0.25, 0.4 };
    float vector2[] = { 0.25, 0.25, 0.25, 0.25 };
    float vector3[] = { 0.1, 0.1, 0.1, 0.7 };
    
    float target[] = { 0.5, 0, 0.4, 0.1 };
    int length = 4;
    
    float error1 = cross_entropy(vector1, target, length);
    float error2 = cross_entropy(vector2, target, length);
    float error3 = cross_entropy(vector3, target, length);
    
    assert_equal_float(error1, 1.7974393);
    assert_equal_float(error2, 1.3862943);
    assert_equal_float(error3, 2.1079940);
    
    return 0;
}

int test_math_softmax(void) {
    int length = 3;
    float neurons[] = { 1.0, 2.0, 3.0 };
    
    softmax(neurons, length);
    
    assert_equal_float(neurons[0], 0.0900305);
    assert_equal_float(neurons[1], 0.2447284);
    assert_equal_float(neurons[2], 0.6652409);
    return 0;
}

int test_math_sqroot(void) {
    float value1 = 2.0;
    float value2 = 100.0;
    float value3 = 15.0;
    float value4 = -15.0;
    
    value1 = sqroot(value1);
    value2 = sqroot(value2);
    value3 = sqroot(value3);
    value4 = sqroot(value4);
    
    assert_equal_float(value1, 1.4142135);
    assert_equal_float(value2, 10);
    assert_equal_float(value3, 3.8729834);
    assert_equal_float(value4, 0);
    
    return 0;
}

int test_math_activate(void) {
    int types_count = 4;
    activation_type types[] = { RELU, TH, LOGISTIC, LINEAR };
    int i;
    for (i = 0; i < types_count; i++) {
        float neuron1 = 1.2;
        float neuron2 = -1.2;
        float neuron3 = 0;
        
        activate(&neuron1, types[i]);
        activate(&neuron2, types[i]);
        activate(&neuron3, types[i]);
        
        switch (types[i]) {
            case RELU: {
                assert_equal_float(neuron1, 1.2);
                assert_equal_float(neuron2, 0);
                assert_equal_float(neuron3, 0);
            }
                break;
            case TH: {
                assert_equal_float(neuron1, 0.8336546);
                assert_equal_float(neuron2, -0.8336546);
                assert_equal_float(neuron3, 0);
            }
                break;
            case LOGISTIC: {
                assert_equal_float(neuron1, 0.7685247);
                assert_equal_float(neuron2, 0.2314752);
                assert_equal_float(neuron3, 0.5);
            }
                break;
            case LINEAR: {
                assert_equal_float(neuron1, 1.2);
                assert_equal_float(neuron2, -1.2);
                assert_equal_float(neuron3, 0);
            }
                break;
            default:
                fail();
                break;
        }
    }
    
    return 0;
}

int test_math_derivative(void) {
    float neuron1 = 1.2;
    float neuron2 = -1.2;
    float neuron3 = 0;
    int types_count = 4;
    activation_type types[] = { RELU, TH, LOGISTIC, LINEAR };
    int i;
    for (i = 0; i < types_count; i++) {
        
        float derivative1 = derivative(neuron1, types[i]);
        float derivative2 = derivative(neuron2, types[i]);
        float derivative3 = derivative(neuron3, types[i]);
        
        switch (types[i]) {
            case RELU: {
                assert_equal_float(derivative1, 1);
                assert_equal_float(derivative2, 0);
                assert_equal_float(derivative3, 0);
            }
                break;
            case TH: {
                assert_equal_float(derivative1, 0.30502);
                assert_equal_float(derivative2, 0.30502);
                assert_equal_float(derivative3, 1);
            }
                break;
            case LOGISTIC: {
                assert_equal_float(derivative1, 0.1778944);
                assert_equal_float(derivative2, 0.1778944);
                assert_equal_float(derivative3, 0.25);
            }
                break;
            case LINEAR: {
                assert_equal_float(derivative1, 1);
                assert_equal_float(derivative2, 1);
                assert_equal_float(derivative3, 1);
            }
                break;
            default:
                fail();
                break;
        }
    }
    
    return 0;
}

int test_math_average(void) {
    float values[] = { 1.0, 2.0, 3.0, 4.0 };
    int length = 4;
    
    float result = average(values, length);
    
    assert_equal_float(result, 2.5);
    return 0;
}

int test_math_min(void) {
    float higher = 10.1;
    float lower = 4.4;
    
    float result = min(higher, lower);
    
    assert_equal_float(result, lower);
    
    return 0;
}

int test_math_max(void) {
    float higher = 10.1;
    float lower = 4.4;
    
    float result = max(higher, lower);
    
    assert_equal_float(result, higher);
    
    return 0;
}
