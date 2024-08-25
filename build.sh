#!/bin/bash

# install necessary packages
sudo apt install -y build-essential

# build program
make

echo "
Program was successfully created as build/ipv4-chat!"
