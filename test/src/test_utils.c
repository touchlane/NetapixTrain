//
//  test_utils.c
//  utils_test
//
//  Created by Pavel Kondrashkov on 5/23/18.
//  Copyright © 2018 Touchlane LLC. All rights reserved.
//

#include "test_utils.h"
#include "unit_test.h"
#include "utils.h"

int test_utils_make_matrix(void) {
    float vector[] = {
        0, 1,
        10, 11
    };
    int r = 2;
    int c = 2;
    float ***matrix = make_matrix(r, c, 1, 0, vector);
    
    assert_equal_float(*(matrix[0][0]), 0);
    assert_equal_float(*(matrix[0][1]), 1);
    assert_equal_float(*(matrix[1][0]), 10);
    assert_equal_float(*(matrix[1][1]), 11);

    free_matrix(matrix, r);
    return 0;
}

int test_utils_make_tensor(void) {
    float vector[] = {
        0, 10, 20, 1, 11, 21, 2, 12, 22, 3, 13, 23
    };
    
    int r = 2;
    int c = 2;
    int d = 3;
    float ****tensor = make_tensor(r, c, d, 0, vector);
    
    assert_equal_float(*(tensor[0][0][0]), 0);
    assert_equal_float(*(tensor[0][0][1]), 1);
    assert_equal_float(*(tensor[0][1][0]), 2);
    assert_equal_float(*(tensor[0][1][1]), 3);
    
    assert_equal_float(*(tensor[1][0][0]), 10);
    assert_equal_float(*(tensor[1][0][1]), 11);
    assert_equal_float(*(tensor[1][1][0]), 12);
    assert_equal_float(*(tensor[1][1][1]), 13);
    
    assert_equal_float(*(tensor[2][0][0]), 20);
    assert_equal_float(*(tensor[2][0][1]), 21);
    assert_equal_float(*(tensor[2][1][0]), 22);
    assert_equal_float(*(tensor[2][1][1]), 23);
    
    free_tensor(tensor, r, d);
    return 0;
}

int test_utils_make_array_of_tensors(void) {
    float vector[] = {
        0,  10,  20,   1,  11,  21,   2,  12,  22,   3, 13,  23,
        100, 110, 120, 101, 111, 121, 102, 112, 122, 103, 113, 123,
        200, 210, 220, 201, 211, 221, 202, 212, 222, 203, 213, 223
    };
    
    int r = 2;
    int c = 2;
    int d = 3;
    int l = 3;
    float *****tensor = make_array_of_tensors(r, c, d, l, vector);
    
    assert_equal_float(*(tensor[0][0][0][0]), 0);
    assert_equal_float(*(tensor[0][0][0][1]), 1);
    assert_equal_float(*(tensor[0][0][1][0]), 2);
    assert_equal_float(*(tensor[0][0][1][1]), 3);
    
    assert_equal_float(*(tensor[0][1][0][0]), 10);
    assert_equal_float(*(tensor[0][1][0][1]), 11);
    assert_equal_float(*(tensor[0][1][1][0]), 12);
    assert_equal_float(*(tensor[0][1][1][1]), 13);
    
    assert_equal_float(*(tensor[0][2][0][0]), 20);
    assert_equal_float(*(tensor[0][2][0][1]), 21);
    assert_equal_float(*(tensor[0][2][1][0]), 22);
    assert_equal_float(*(tensor[0][2][1][1]), 23);
    
    
    assert_equal_float(*(tensor[1][0][0][0]), 100);
    assert_equal_float(*(tensor[1][0][0][1]), 101);
    assert_equal_float(*(tensor[1][0][1][0]), 102);
    assert_equal_float(*(tensor[1][0][1][1]), 103);
    
    assert_equal_float(*(tensor[1][1][0][0]), 110);
    assert_equal_float(*(tensor[1][1][0][1]), 111);
    assert_equal_float(*(tensor[1][1][1][0]), 112);
    assert_equal_float(*(tensor[1][1][1][1]), 113);
    
    assert_equal_float(*(tensor[1][2][0][0]), 120);
    assert_equal_float(*(tensor[1][2][0][1]), 121);
    assert_equal_float(*(tensor[1][2][1][0]), 122);
    assert_equal_float(*(tensor[1][2][1][1]), 123);
    
    
    assert_equal_float(*(tensor[2][0][0][0]), 200);
    assert_equal_float(*(tensor[2][0][0][1]), 201);
    assert_equal_float(*(tensor[2][0][1][0]), 202);
    assert_equal_float(*(tensor[2][0][1][1]), 203);
    
    assert_equal_float(*(tensor[2][1][0][0]), 210);
    assert_equal_float(*(tensor[2][1][0][1]), 211);
    assert_equal_float(*(tensor[2][1][1][0]), 212);
    assert_equal_float(*(tensor[2][1][1][1]), 213);
    
    assert_equal_float(*(tensor[2][2][0][0]), 220);
    assert_equal_float(*(tensor[2][2][0][1]), 221);
    assert_equal_float(*(tensor[2][2][1][0]), 222);
    assert_equal_float(*(tensor[2][2][1][1]), 223);
    
    free_array_of_tensors(tensor, r, d, l);
    return 0;
}
