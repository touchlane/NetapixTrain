//
//  test.c
//  netapix
//
//  Created by Pavel Kondrashkov on 5/22/18.
//  Copyright © 2018 Touchlane LLC. All rights reserved.
//

#include <stdio.h>
#include "unit_test.h"
#include "test_options.h"

int main() {
    test(test_options_determine_run_mode_defined, "utils test determine run mode defined.");
    test(test_options_determine_run_mode_not_defined, "utils test determine run mode not defined.");
    test(test_options_determine_run_mode_not_specified, "utils test determine run mode not specified.");
    test(test_options_remove_ext, "utils test remove file extension from path.");
    test(test_options_last_path_component, "utils test last path component from path.");
    test(test_options_make_output_save_path, "utils test make output save path.");

    printf("PASSED: %d\nFAILED: %d\n", test_passed, test_failed);
    return (test_failed > 0);
}
