//
//  constants.h
//  netapix
//
//  Created by Pavel Kondrashkov on 6/27/18.
//  Copyright © 2018 Touchlane LLC. All rights reserved.
//

#ifndef constants_h
#define constants_h

#define FATAL_ERROR_NETAPIX_MODE_USAGE_FAIL_MSG "usage: %s [train | run]\n"
#define FATAL_ERROR_NETAPIX_MODE_TRAIN_USAGE_FAIL_MSG "usage: %s %s [path/config.npw] [path/dataset/*.npt] [path/weights.npw (optional)]\n"
#define FATAL_ERROR_NETAPIX_MODE_RUN_USAGE_FAIL_MSG "usage: %s %s [path/input.npi] [path/weights.npw] [path/output/ (optional)]\n"
#define FATAL_ERROR_REMOVING_OUTPUT_DIRECTORY_FAIL_MSG "Could not remove output directory %s.\n"
#define FATAL_ERROR_COPY_FILE_FAIL_MSG "Could not copy file from %s to %s.\n"

#define ERROR_OUTPUT_DIRECTORY_EXISTS_MSG "Output directory %s exists. Override? Y\\N\n"
#define DEFAULT_OUTPUT_WEIGHTS_DIRECTORY_NAME "weights"
#define DEFAULT_OUTPUT_WEIGHTS_PARAMS_DIRECTORY_NAME "weights/params"
#define DEFAULT_OUTPUT_RUN_DIRECTORY_NAME "output"

#endif /* constants_h */
