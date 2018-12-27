#!/bin/bash
set -e

mkdir -p build/
cd build/
cmake ..
thread_num=$(nproc)
make -j$((++thread_num))
cd ..
