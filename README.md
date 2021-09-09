![LOGO](https://github.com/touchlane/Netapix/blob/master/assets/logo.svg)

[![Build Status](https://travis-ci.org/touchlane/Netapix.svg?branch=master)](https://travis-ci.org/touchlane/NetapixTrain)
[![codecov.io](https://codecov.io/gh/touchlane/Netapix/branch/master/graph/badge.svg)](https://codecov.io/gh/codecov/NetapixTrain/branch/master)
![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Platform](https://img.shields.io/badge/platform-MacOS-lightgrey.svg)

NetapixTrain is a neural network framework written on pure C. For now it supports CPU mode only. The purpose of the project is investigation of  deep neural networks behavior and designing new and effective architectures of convolutional networks.

# Installation
```
make
```

# Usage

### Train brand new model
```
./example/bin/netapix train [NPX_PATH] [NPT_PATH]  
```
Folders `weights` and `weights/params` will be created at the `[NPX_PATH]`.   
- `weights` for trained weights `.npw` file.   
- `weights/params` for copied configuration `.npx` file.


### Continue training existing model
```
./example/bin/netapix train [NPX_PATH] [NPT_PATH] [NPW_PATH]  
```
Folders `weights` and `weights/params` will be created at the `[NPW_PATH]`.   
- `weights` for new trained weights `.npw` file.   
- `weights/params` for copied configuration `.npx` and `.npw` files that were used to continue training.


### Test the model
```
./example/bin/netapix run [NPI_PATH] [NPW_PATH] [OUTPUT_PATH]  
```
Output file `.npo` will be created under `[OUTPUT_PATH]` folder.  
`[OUTPUT_PATH]` is optional. Default value at executable filepath – `./example/bin/output/`.


# Documentation

## Formats

Netapix works with custom formats. There are number of tools for conversion available [here](https://github.com/touchlane/NetapixTools)

### .npx
Special format to define network structure and learning policy. See example of simple convolutional network for MNIST dataset below:

```
[config]
batch=32
threads=4
channels=1
width=28
height=28
init=xavier
validation=10
backup=5000
learning=gradient
regularization=L2
accuracy=0.00001
eta=0.01
momentum=0
lambda=0
alpha=0.99
beta=1.01
gamma=1.01

[convolutional]
width=14
height=14
channels=100
stride=1
padding=0
activation=relu

[convolutional]
width=14
height=14
channels=10
stride=1
padding=0
activation=relu

[convolutional]
width=2
height=2
channels=10
stride=1
padding=0
activation=relu

[loss]
input=10
activation=msqe
```

### .npw

Particular format for binary files with weights. Every .npw file follows the same structure:
```
[number of layers][4 bytes]

[layer config] [4 bytes * 15]
[weights] [4 bytes * n]
[biases] [4 bytes * m]

[layer config] [4 bytes * 15]
[weights] [4 bytes * n_1]
[biases] [4 bytes * m_1]
.
.
.
[layer config] [4 bytes * 15]
[weights] [4 bytes * n_k]
[biases] [4 bytes * m_l]
```

### .npt

Distinct file format for train binary files of the NetapixTrain framework. It consists of two consequtive one-dimensional arrays with float32.

### .npi

Custom format for input binary files of the NetapixTrain framework. Contains two consequtive one-dimensional arrays with numbers of float32 format.

### .npo

Peculiar format for output binary files of the NetapixTrain framework. This format has one-dimensional array of 4-byte float inside.

## Config

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

### Activation

| Key |  Comment |
| ------------- | ------------- |
| **linear** | *f(x) = x* |
| **relu** | *f(x) > 0 ? x : 0* |
| **logistic**| the standard logistic function|
| **th**| the hyperbolic tangent |
| **softmax**| the normalized exponential function |

### Loss

| Key |  Comment |
| ------------- | ------------- |
| **msqe** | mean squared error |
| **entropy** | cross entropy |

# Tools

Subsidiary tools for the NetapixTrain framework. These tools are all you need in order to be a full-fledged user.

| Tool | Comment |
| ------------- | ------------- |
| [.jpg + .txt => .npt](https://github.com/touchlane/NetapixTrain/tree/jpg_and_txt_to_npt) | converts .jpg and .txt pair to .npt format |
| [.jpg + .jpg => .npt](https://github.com/touchlane/NetapixTrain/tree/jpg_and_jpg_to_npt) | converts .jpg and .jpg pair to .npt format |
| [.jpg => .npi](https://github.com/touchlane/NetapixTrain/tree/jpg_to_npi) | converts .jpg to .npi |
| [.npo => .txt](https://github.com/touchlane/NetapixTrain/tree/npo_to_txt) | converts .npo to .txt |
| [.npo => .png](https://github.com/touchlane/NetapixTrain/tree/npo_to_png) | converts .npo to .png |
| [loss graph](https://github.com/touchlane/NetapixTrain/tree/loss_graph) | training progress visualisation |
| [batch run](https://github.com/touchlane/NetapixTrain/tree/batch_run_script) | app execution for multiple inputs |
| [test accuracy](https://github.com/touchlane/NetapixTrain/tree/accuracy_test) | model accuracy evaluation |
| [image normalizer](https://github.com/touchlane/NetapixTrain/tree/image_normalizer) | images adjusting |
| [.npw reader](https://github.com/touchlane/NetapixTrain/tree/npw_reader) | looking inside weight files  |
| [.png => .jpg](https://github.com/touchlane/NetapixTrain/tree/png_to_jpg) | simple convertation .png to .jpg 
