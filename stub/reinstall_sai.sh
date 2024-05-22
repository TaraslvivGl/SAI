#!/bin/bash

make clean
./autoclean.sh
./autogen.sh
./configure
make && sudo make install
