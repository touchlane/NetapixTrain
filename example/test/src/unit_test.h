//
//  unit_test.h
//  netapix
//
//  Created by Pavel Kondrashkov on 5/24/18.
//  Copyright © 2018 Touchlane LLC. All rights reserved.
//

#ifndef unit_test_h
#define unit_test_h

#include <math.h>
#include <string.h>

extern int test_passed;
extern int test_failed;

/* Terminate current test with error */
#define fail() return __LINE__

/* Successful end of the test case */
#define done() return 0

/* Assert defines */
#define assert(cond) do { if (!(cond)) fail(); } while (0)
#define assert_not_equal(a, b) assert( a != b )
#define assert_equal(a, b) assert( a == b )
#define assert_equal_float(a, b) assert( (fabs(a - b) < 1e-7) )
#define assert_equal_string(a, b) assert( !strcmp((const char*)a, (const char*)b) )
#define assert_equal_ptr(a, b) assert( (void*)(a) == (void*)(b))

/* Test runner */
void test(int (*func)(void), const char *name);

#endif /* unit_test_h */
