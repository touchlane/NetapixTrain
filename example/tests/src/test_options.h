//
//  test_options.h
//  netapix
//
//  Created by Pavel Kondrashkov on 6/27/18.
//  Copyright © 2018 Touchlane LLC. All rights reserved.
//

#ifndef test_options_h
#define test_options_h

#include <stdio.h>

int test_options_determine_run_mode_defined(void);
int test_options_determine_run_mode_not_defined(void);
int test_options_determine_run_mode_not_specified(void);
int test_options_remove_ext(void);
int test_options_last_path_component(void);
int test_options_make_output_save_path(void);

#endif /* test_options_h */
