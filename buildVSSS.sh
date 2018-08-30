#!/bin/bash
set -e

mkdir build/
cd build/
cmake ..
thread_num=$(nproc)
make -j$((++thread_num))
cd ..
