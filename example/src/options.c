//
//  options.c
//  netapix
//
//  Created by Pavel Kondrashkov on 6/27/18.
//  Copyright © 2018 Touchlane LLC. All rights reserved.
//

#include "options.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <ftw.h>
#include <dirent.h>

#include "constants.h"

netapix_mode determine_run_mode(int argc, char *argv[]) {
    if (argc < 2) {
        return NDF_MODE;
    }
    if (0 == strcmp(argv[1], "train")) {
        return TRAIN_MODE;
    } else if (0 == strcmp(argv[1], "run")) {
        return RUN_MODE;
    }
    return NDF_MODE;
}

char *remove_ext(char *path) {
    size_t len = strlen(path);
    size_t i;
    size_t dot_index = -1;
    char *copy_str = malloc((len + 1) * sizeof(*copy_str));
    strcpy(copy_str, path);
    for (i = 0; i < len; ++i) {
        if (path[i] == '.') {
            dot_index = i;
        }
    }
    if (dot_index != -1) {
        copy_str[dot_index] = '\0';
    }
    return copy_str;
}

char *last_path_component(char *path) {
    char *component = strrchr(path, '/');
    return component ? component + 1 : path;
}

char *remove_last_path_component(char *path) {
    size_t len = strlen(path);
    char *copy_str = malloc((len + 1) * sizeof(*copy_str));
    strcpy(copy_str, path);
    char *component = strrchr(copy_str, '/');
    *component = '\0';
    return copy_str;
}

int unlink_cb(const char *path, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    int rv = remove(path);
    if (rv) {
        printf(FATAL_ERROR_REMOVING_OUTPUT_DIRECTORY_FAIL_MSG, path);
    }
    return rv;
}

int prepare_output_path(char *output_path, int force_create) {
    if (!force_create && 0 == directory_exists(output_path)) {
        char answer;
        printf(ERROR_OUTPUT_DIRECTORY_EXISTS_MSG, output_path);
        do { answer = fgetc(stdin); } while (answer != 'Y' && answer != 'N');
        if (answer == 'N') {
            return 1;
        }
        nftw(output_path, unlink_cb, 64, FTW_DEPTH | FTW_PHYS);
    }
    mkdir(output_path, S_IRWXU);
    return 0;
}

char *make_output_save_path(char *base_path, char *directory_name) {
    char *output_base_path = remove_last_path_component(base_path);
    char *output_path = malloc( (strlen(output_base_path) +
                                 strlen(directory_name) +
                                 strlen("//") + 1) * sizeof(*output_path) ); /// + 1 char for the null char.
    sprintf(output_path, "%s/%s/", output_base_path, directory_name);
    return output_path;
}

int copy_param_files(char *config_file_path, char *weights_file_path, char *output_path) {
    if (config_file_path) {
        char *config_file_name = last_path_component(config_file_path);
        char *config_copy_file_path = malloc((strlen(output_path) +
                                              strlen(config_file_name) + 1) * sizeof(char));
        sprintf(config_copy_file_path, "%s%s", output_path, config_file_name);
        if (copy_file(config_file_path, config_copy_file_path)) {
            return 1;
        }
    }
    if (weights_file_path) {
        char *weights_file_name = last_path_component(weights_file_path);
        char *weights_copy_file_path = malloc((strlen(output_path) +
                                               strlen(weights_file_name) + 1) * sizeof(char));
        sprintf(weights_copy_file_path, "%s%s", output_path, weights_file_name);
        if (copy_file(weights_file_path, weights_copy_file_path)) {
            return 1;
        }
    }
    return 0;
}

int copy_file(char *from_path, char *to_path) {
    FILE *from_file, *to_file;
    from_file = fopen(from_path, "rb");
    if (!from_file) {
        return 1;
    }
    to_file = fopen(to_path, "wb");
    if (!to_file) {
        fclose(from_file);
        return 1;
    }
    size_t read_data, write_data;
    unsigned char buff[8192];
    do {
        read_data = fread(buff, 1, sizeof(buff), from_file);
        if (read_data) {
            write_data = fwrite(buff, 1, read_data, to_file);
        } else {
            write_data = 0;
        }
    } while ((read_data > 0) && (read_data == write_data));
    if (write_data) {
        fclose(from_file);
        fclose(to_file);
        return 1;
    }
    fclose(from_file);
    fclose(to_file);
    return 0;
}

int directory_exists(char *path) {
    DIR *dir = opendir(path);
    if (dir) {
        /// Directory exists.
        closedir(dir);
        return 0;
    }
    return 1;
}
