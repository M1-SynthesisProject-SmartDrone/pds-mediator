#!/bin/sh

# This is a simple compiler script that will create an executable in the "bin" folder

sudo mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j
cd ..