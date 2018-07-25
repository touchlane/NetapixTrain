//
//  test_loss.h
//  test
//
//  Created by Pavel Kondrashkov on 5/28/18.
//  Copyright © 2018 Touchlane LLC. All rights reserved.
//

#ifndef test_loss_h
#define test_loss_h

#include <stdio.h>

int test_loss_make_loss_layer(void);
int test_loss_forward(void);
int test_loss_backward_scalar_msqe(void);
int test_loss_backward_softmax_msqe(void);
int test_loss_backward_scalar_cross_entropy(void);
int test_loss_backward_softmax_cross_entropy(void);

#endif /* test_loss_h */
