![LOGO](https://github.com/touchlane/Netapix/blob/master/assets/logo.svg)

[![Build Status](https://travis-ci.org/touchlane/Netapix.svg?branch=master)](https://travis-ci.org/touchlane/Netapix)
[![codecov.io](https://codecov.io/gh/touchlane/Netapix/branch/master/graph/badge.svg)](https://codecov.io/gh/codecov/Netapix/branch/master)
![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Platform](https://img.shields.io/badge/platform-MacOS-lightgrey.svg)

Netapix is a neural network framework written on pure C. For now it supports CPU mode only. The purpose of the project is investigation of  deep neural networks behavior and designing new and effective architectures of convolutional networks.

# Installation
```
make
```

# Usage

### Train brand new model
```
./example/bin/netapix train [NPX_PATH] [NPT_PATH]  
```

### Continue training existing model
```
./example/bin/netapix train [NPX_PATH] [NPT_PATH] [NPW_PATH]  
```

### Test the model
```
./example/bin/netapix run [NPI_PATH] [NPW_PATH] [OUTPUT_PATH]  
```

# Documentation
Brief overview of the implemented functionality and some insights.

## Formats

### .npx

### .npw

### .npt

### .npi

### .npo

## Config
Available options to configure training process.

| Key |  Comment |
| ------------- | ------------- |
|**threads** | number of availbale CPU threads |
|**batch** | total number of training examples present in a single batch  |
|**channels** | the depth of the input tenzor (for networks having the first layer as convolutional) |
|**width** | the width of the input tenzor (for networks having the first layer as convolutional) | 
|**height** | the height of the input tenzor (for networks having the first layer as convolutional) | 
|**init** | the weights initialization type |
|**validation** | indicates the part of the training set reserved for the cross validation |
|**backup** | weights save rate |
|**learning** | supported optimiziers (**gradient**) |
|**regularization** |regularization type (**L1** or **L2**)|
|**accuracy** | the target occuracy |
|**eta** | start learning rate |
|**momentum** | momentum coefficient|
|**lambda** | regularization coefficient |
|**alpha** | decrease learning rate coefficient|
|**beta** |  increase learning rate coefficient |
|**gamma** | delta error correction coefficient |

## Layers
Supported layers.

### Connected
| Key |  Comment |
| ------------- | ------------- |
| input | the output's size of the previous layer |
| activation | the type of the activation function |

### Convolutional
| Key |  Comment |
| ------------- | ------------- |
| width | filter width |
| height | filter height |
| channels | number of filters |
| stride | controls how the filter convolves around the input volume |
| padding | positive integer to define the central kernel element |
| activation | the type of the activation function |

### Loss
| Key |  Comment |
| ------------- | ------------- |
| **input** | the size of the networks's output |
| **activation** | the type of a loss function |

## Math

### Layers's activations

Supported functions for layer activation

| Key |  Comment |
| ------------- | ------------- |
| **linear** | *f(x) = x* |
| **relu** | *f(x) > 0 ? x : 0* |
| **logistic**| |
| **th**| |
| **softmax**| |

