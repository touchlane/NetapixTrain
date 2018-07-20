//
//  utils.c
//  netapix
//
//  Created by Evgeny Dedovets on 4/23/18.
//  Copyright © 2018 Touchlane LLC. All rights reserved.
//

#include "utils.h"
#include <stdio.h>
#include <string.h>

int read_txt(const char *filename, char **buffer, unsigned long *length) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        printf(ERROR_OPEN_FILE_MSG, filename);
        return ERROR_OPEN;
    }
    fseek(file, 0, SEEK_END);
    *length = ftell(file);
    fseek(file, 0, SEEK_SET);
    *buffer = malloc(sizeof(char) * (*length));
    fread(*buffer, *length, sizeof(char), file);
    fclose(file);
    return 0;
}


float ***make_matrix(int r, int c, int stride, int offset,  float *val) {
    float ***m = malloc(r * sizeof(*m));
    int i, j;
    for (i = 0; i < r; i++) {
        m[i] = malloc(c * sizeof(**m));
        for (j = 0; j < c; j++) {
            m[i][j] = &val[stride * (j + i * c) + offset];
        }
    }
    return m;
}

int free_matrix(float ***m, int r) {
    int i;
    for (i = 0; i < r; i++) {
        free(m[i]);
    }
    free(m);
    return 0;
}

float ****make_tensor(int r, int c, int d, int offset, float *val) {
    float ****t = malloc(d * sizeof(*t));
    int i;
    for (i = 0; i < d; i++) {
        float ***matrix = make_matrix(r, c, d, i + offset * r * c * d, val);
        t[i] = matrix;
    }
    return t;
}

int free_tensor(float ****t, int r, int d) {
    int i;
    for (i = 0; i < d; i++) {
        free_matrix(t[i], r);
    }
    free(t);
    return 0;
}

float *****make_array_of_tensors(int r, int c, int d, int l, float *val) {
    float *****a = malloc(l * sizeof(*a));
    int i;
    for (i = 0; i < l; i++) {
        a[i] = make_tensor(r, c, d, i, val);
    }
    return a;
}

int free_array_of_tensors(float *****a, int r, int d, int l) {
    int i;
    for (i = 0; i < l; i++) {
        free_tensor(a[i], r, d);
    }
    free(a);
    return 0;
}
