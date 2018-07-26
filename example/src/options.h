//
//  options.h
//  netapix
//
//  Created by Pavel Kondrashkov on 6/27/18.
//  Copyright © 2018 Touchlane LLC. All rights reserved.
//

#ifndef options_h
#define options_h

#include <stdio.h>

typedef enum {
    TRAIN_MODE,
    RUN_MODE,
    NDF_MODE
} netapix_mode;

netapix_mode determine_run_mode(int argc, char *argv[]);
char *make_output_save_path(char *base_path, char *output_name);
char *remove_ext(char *path);
char *last_path_component(char *path);
int directory_exists(char *path);
int prepare_output_path(char *output_path, int force_create);
int copy_file(char *from_path, char *to_path);
int copy_param_files(char *config_file_path, char *weights_file_path, char *output_path);

#endif /* options_h */
