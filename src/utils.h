//
//  utils.h
//  netapix
//
//  Created by Evgeny Dedovets on 4/23/18.
//  Copyright © 2018 Touchlane LLC. All rights reserved.
//

#ifndef utils_h
#define utils_h

#include <stdlib.h>
#include "constants.h"

int read_txt(const char *filename, char **buffer, unsigned long *length);
float *****make_array_of_tensors(int r, int c, int d, int l, float *val);
int free_array_of_tensors(float *****a, int r, int d, int l);
float ****make_tensor(int r, int c, int d, int offset, float *val);
int free_tensor(float ****t, int r, int d);
float ***make_matrix(int r, int c, int stride, int offset,  float *val);
int free_matrix(float ***m, int r);

#endif /* utils_h */
