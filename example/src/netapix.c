//
//  main.c
//  Parser
//
//  Created by Evgeny Dedovets on 4/20/18.
//  Copyright © 2018 Touchlane LLC. All rights reserved.
//

#include "netapix.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "options.h"
#include "constants.h"

int train_mode(int argc, char *argv[]);
int run_mode(int argc, char *argv[]);

int main(int argc, char *argv[]) {
    netapix_mode mode = determine_run_mode(argc, argv);
    switch (mode) {
        case TRAIN_MODE:
            train_mode(argc, argv);
            break;
        case RUN_MODE:
            run_mode(argc, argv);
            break;
        case NDF_MODE:
            printf(FATAL_ERROR_NETAPIX_MODE_USAGE_FAIL_MSG, argv[0]);
            return 0;
    }
    return 0;
}

int train_mode(int argc, char *argv[]) {
    if (argc < 4){
        printf(FATAL_ERROR_NETAPIX_MODE_TRAIN_USAGE_FAIL_MSG, argv[0], argv[1]);
        return 1;
    }
    
    char *npx_path = argv[2];
    char *train_path = argv[3];
    char *weights_path = (argc > 4) ? argv[4] : NULL;
    
    char *params_save_path = NULL;
    char *output_path = NULL;
    if (weights_path) {
        output_path = make_output_save_path(weights_path, DEFAULT_OUTPUT_WEIGHTS_DIRECTORY_NAME);
        params_save_path = make_output_save_path(weights_path, DEFAULT_OUTPUT_WEIGHTS_PARAMS_DIRECTORY_NAME);
    } else {
        output_path = make_output_save_path(npx_path, DEFAULT_OUTPUT_WEIGHTS_DIRECTORY_NAME);
        params_save_path = output_path;
    }
    
    if (prepare_output_path(output_path, 0) || prepare_output_path(params_save_path, 1)) {
        return 0;
    }
    if (copy_param_files(npx_path, weights_path, params_save_path)) {
        return 0;
    }
    
    train(npx_path, train_path, weights_path, output_path);
    return 0;
}

int run_mode(int argc, char *argv[]) {
    if (argc < 4){
        printf(FATAL_ERROR_NETAPIX_MODE_RUN_USAGE_FAIL_MSG, argv[0], argv[1]);
        return 1;
    }
    char *input_path = argv[2];
    char *weights_path = argv[3];
    char *output_path = (argc > 4) ? argv[4] : NULL;
    
    output_path = make_output_save_path(output_path ? output_path : "./", DEFAULT_OUTPUT_RUN_DIRECTORY_NAME);
    if (prepare_output_path(output_path, 1)) {
        return 0;
    }
    
    char *input_name = remove_ext(last_path_component(input_path));
    output_path = realloc(output_path, (strlen(output_path) +
                                        strlen(input_name) +
                                        strlen(".npo") + 1) * sizeof(*output_path));
    sprintf(output_path, "%s%s.npo", output_path, input_name);
    
    run(input_path, weights_path, output_path);
    return 0;
}
