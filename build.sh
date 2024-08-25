#!/bin/bash

# add launch privileges to script
chmod u+x build.sh

# install necessary packages
sudo apt install -y build-essential

# build program
make

echo "
Program was successfully created as build/ipv4-chat!"
