#!/bin/bash

# add launch privileges to script
chmod u+x start_chat.sh

# install necessary packages
sudo apt install build-essential

# build program
make

# go to build directory
cd build
