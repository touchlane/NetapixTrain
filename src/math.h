//
//  math.h
//  netapix
//
//  Created by Evgeny Dedovets on 12/11/17.
//  Copyright © 2017 Touchlane LLC. All rights reserved.
//

#ifndef math_h
#define math_h

#include "config.h"

#define MINDIFF 2.25e-308 // use for convergence check

void activate(float *neuron, activation_type type);
float derivative(float neuron, activation_type type);
float linear(float neuron);
float derivative_linear(float neuron);
float relu(float neuron);
float derivative_relu(float neuron);
float th(float neuron);
float derivative_th(float neuron);
float logistic(float neuron);
float derivative_logistic(float neuron);
void softmax(float *neurons, int lenght);
float msqe(float *vector, float *target, int lenght);
float cross_entropy(float *vector, float *target, int lenght);
float sqroot(float square);
float rand_uniform(float min, float max);
int rand_int(int min, int max);
float average(float *val, int lenght);
float min(float left, float right);
float max(float left, float right);
float detect_max(float *val, int length);

#endif /* math_h */
