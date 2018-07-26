//
//  unit_test.c
//  netapix
//
//  Created by Pavel Kondrashkov on 5/24/18.
//  Copyright © 2018 Touchlane LLC. All rights reserved.
//

#include <stdio.h>
#include "unit_test.h"

int test_passed = 0;
int test_failed = 0;

void test(int (*func)(void), const char *name) {
    int r = func();
    if (r == 0) {
        test_passed++;
    } else {
        test_failed++;
        printf("FAILED: %s (at line %d)\n", name, r);
    }
}
