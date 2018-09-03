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

### .npi

### .npo

## Config

