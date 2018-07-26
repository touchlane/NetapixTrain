//
//  netapix.h
//  netapix
//
//  Created by Pavel Kondrashkov on 5/29/18.
//  Copyright © 2018 Touchlane LLC. All rights reserved.
//

#ifndef netapix_h
#define netapix_h

int train(char *npx_path, char *train_path, char *weights_path, char *output_path);
int run(char *npi_path, char *weights_path, char *output_path);

#endif /* netapix_h */
