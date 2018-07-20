//
//  math.c
//  netapix
//
//  Created by Evgeny Dedovets on 12/11/17.
//  Copyright © 2017 Touchlane LLC. All rights reserved.
//

#include "math.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "constants.h"

const float min_log_arg = 1e-15;
const float max_exp_arg = 85.0;

void softmax(float *neurons, int lenght) {
    float denominator = 0;
    int i;
    float max_val = detect_max(neurons, lenght);
    for (i = 0; i < lenght; i++) {
        denominator = denominator + expf(min(neurons[i] - max_val, max_exp_arg));
    }
    for (i = 0; i < lenght; i++) {
        neurons[i] = expf(min(neurons[i] - max_val, max_exp_arg)) / denominator;
    }
}

float linear(float neuron) {
    return neuron;
}

float derivative_linear(float neuron) {
    return 1;
}

float relu(float neuron) {
    if (neuron < 0) {
        return 0;
    }
    return neuron;
}

float derivative_relu(float neuron) {
    if (neuron > 0) {
        return 1;
    }
    return 0;
}

float th(float neuron) {
    float n = max(min(neuron, max_exp_arg), -max_exp_arg);
    return (expf(n) - expf(-n)) / (expf(n) + expf(-n));
}

float derivative_th(float neuron) {
    return (1 + th(neuron))*(1 - th(neuron));
}

float logistic(float neuron) {
    float n = min(neuron, max_exp_arg);
    return expf(n) / (1 + expf(n));
}

float derivative_logistic(float neuron) {
    return logistic(neuron)*(1 - logistic(neuron));
}

float msqe(float *vector, float *target, int lenght) {
    float error = 0;
    float tmp = 0;
    for (int i = 0; i < lenght; i++) {
        tmp = target[i] - vector[i];
        error = error + tmp*tmp;
    }
    return error/2;
}

float cross_entropy(float *vector, float *target, int lenght) {
    float error = 0;
    for (int i = 0; i < lenght; i++) {
        error = error + target[i] * logf(max(vector[i], min_log_arg));
    }
    return -error;
}

void activate(float *neuron, activation_type type) {
    switch (type) {
        case RELU:
            *neuron = relu(*neuron);
            break;
        case TH:
            *neuron = th(*neuron);
            break;
        case LOGISTIC:
            *neuron = logistic(*neuron);
            break;
        case LINEAR:
            *neuron = linear(*neuron);
            break;
        default:
            printf(FATAL_ERROR_ACTIVATION_FAIL_MSG);
            exit(1);
            break;
    }
}

float derivative(float neuron, activation_type type) {
    float derivative = 0;
    switch (type) {
        case RELU:
            derivative = derivative_relu(neuron);
            break;
        case TH:
            derivative = derivative_th(neuron);
            break;
        case LOGISTIC:
            derivative = derivative_logistic(neuron);
            break;
        case LINEAR:
            derivative = derivative_linear(neuron);
            break;
        default:
            printf(FATAL_ERROR_ACTIVATION_FAIL_MSG);
            exit(1);
            break;
    }
    return derivative;
}

float sqroot(float square) {
    float root = square / 3, last, diff = 1;
    if (square <= 0) return 0;
    do {
        last = root;
        root = (root + square / root) / 2;
        diff = root - last;
    } while (diff > MINDIFF || diff < -MINDIFF);
    return root;
}

float rand_uniform(float min, float max) {
    if (max < min) {
        float swap = min;
        min = max;
        max = swap;
    }
    return ((float)rand() / RAND_MAX * (max - min)) + min;
}

int rand_int(int min, int max) {
    if (max < min){
        int s = min;
        min = max;
        max = s;
    }
    int r = (rand()%(max - min + 1)) + min;
    return r;
}

float average(float *val, int lenght) {
    float res = 0;
    for (int i = 0; i < lenght; i++) {
        res = res + val[i];
    }
    return res / (float)lenght;
}

float min(float left, float right) {
    return left < right ? left : right;
}

float max(float left, float right) {
    return left > right ? left : right;
}

float detect_max(float *val, int length) {
    float max = val[0];
    for (int i = length - 1; i > 0; i--) {
        if (val[i] > max) {
            max = val[i];
        }
    }
    return max;
}
