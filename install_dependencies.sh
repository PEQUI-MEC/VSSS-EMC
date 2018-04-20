#!/bin/bash
set -e

while true; do
    read -p "Do you wish to install CUDA (CUDA only works with a Nvidia GPU)? [y/n] \n" yn
    case $yn in
        [Yy]* ) sh install_cuda.sh; break;;
        [Nn]* ) break;; then break
        * ) echo "Please answer yes or no.";;
    esac
done

mkdir dependencies/
cd dependencies/

echo "Installing Boost"
sudo apt-get install libboost-all-dev

echo "Installing Lib4L"
sudo apt-get install libv4l-dev libv4lconvert0

echo "Installing GTKmm"
sudo apt-get install libgtkmm-3.0-dev

echo "Installing OpenCV"
git clone -b 3.4.1 https://github.com/opencv/opencv.git
cd opencv/
mkdir build/
cd build/
cmake -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local -D WITH_CUDA=ON -D WITH_CUBLAS=ON -D BUILD_PERF_TESTS=OFF -D BUILD_TESTS=OFF -D BUILD_opencv_java=OFF -D CUDA_GENERATION=Auto -D CUDA_NVCC_FLAGS="-D_FORCE_INLINES" ..
thread_num=$(nproc)
make -j$((++thread_num))
sudo make install
cd ../../
rm -rf opencv

echo "Installing libxbee3"
git clone https://github.com/lopelope/libxbee3.git
cd libxbee3/
git checkout waitforack-needsfree-issues-fix
make configure
make
sudo make install
cd ../
rm -rf libxbee3

cd ../
rm -rf dependencies/

echo "All dependencies were installed. Run ./buildVSSS.sh to build the project."
