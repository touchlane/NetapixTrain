//
//  test_utils.c
//  netapix
//
//  Created by Pavel Kondrashkov on 6/27/18.
//  Copyright © 2018 Touchlane LLC. All rights reserved.
//

#include "test_utils.h"
#include "unit_test.h"
#include "utils.h"

int test_utils_determine_run_mode_defined(void) {
    int argc = 2;
    char *argv[] = { "netapix", "train" };
    
    netapix_mode mode = determine_run_mode(argc, argv);
    
    assert_equal(mode, TRAIN_MODE);
    return 0;
}

int test_utils_determine_run_mode_not_defined(void) {
    int argc = 2;
    char *argv[] = { "netapix", "somemode" };
    
    netapix_mode mode = determine_run_mode(argc, argv);
    
    assert_equal(mode, NDF_MODE);
    return 0;
}

int test_utils_determine_run_mode_not_specified(void) {
    int argc = 1;
    char *argv[] = { "netapix" };
    
    netapix_mode mode = determine_run_mode(argc, argv);
    
    assert_equal(mode, NDF_MODE);
    return 0;
}

int test_utils_remove_ext(void) {
    char str1[] = "../../path/folder/config.npx";
    char str2[] = "./config.npx";
    char str3[] = "/config.npx";
    char str4[] = "config";
    
    char *result1 = remove_ext(str1);
    char *result2 = remove_ext(str2);
    char *result3 = remove_ext(str3);
    char *result4 = remove_ext(str4);
    
    assert_equal_string(result1, "../../path/folder/config");
    assert_equal_string(result2, "./config");
    assert_equal_string(result3, "/config");
    assert_equal_string(result4, "config");
    return 0;
}

int test_utils_last_path_component(void) {
    char str1[] = "../../path/folder/config.npx";
    char str2[] = "./config.npx";
    char str3[] = "/config.npx";
    char str4[] = "config.npx";
    
    char *result1 = last_path_component(str1);
    char *result2 = last_path_component(str2);
    char *result3 = last_path_component(str3);
    char *result4 = last_path_component(str4);
    
    assert_equal_string(result1, "config.npx");
    assert_equal_string(result2, "config.npx");
    assert_equal_string(result3, "config.npx");
    assert_equal_string(result4, "config.npx");
    return 0;
}

int test_utila_make_output_save_path(void) {
    char *base_path = "./";
    char *output_name = "output";
    
    char *result = make_output_save_path(base_path, output_name);
    
    assert_equal_string(result, "./output/");
    return 0;
}
