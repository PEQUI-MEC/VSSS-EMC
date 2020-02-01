#!/bin/bash
set -e

mkdir dependencies/
cd dependencies/

echo "Installing OpenCV"
git clone -b 4.0.1 https://github.com/opencv/opencv.git
cd opencv/
mkdir build/
cd build/
cmake -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local -D WITH_CUBLAS=ON -D BUILD_PERF_TESTS=OFF -D BUILD_TESTS=OFF -D BUILD_opencv_java=OFF -D CUDA_GENERATION=Auto -D CUDA_NVCC_FLAGS="-D_FORCE_INLINES" ..
thread_num=$(nproc)
make -j$((++thread_num))
sudo make install
cd ../../
rm -rf opencv

cd ../
rm -rf dependencies/

echo "All dependencies were installed. Run ./buildVSSS.sh to build the project."
