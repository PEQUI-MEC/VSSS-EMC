#!/bin/bash
set -e

mkdir dependencies/
cd dependencies/

echo "Installing compilation tools"
sudo apt-get install build-essential cmake -y

echo "Installing gcc-8"
sudo add-apt-repository ppa:ubuntu-toolchain-r/test
sudo apt-get update
sudo apt-get install gcc-8 g++-8 -y
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-7 700 --slave /usr/bin/g++ g++ /usr/bin/g++-7
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-8 800 --slave /usr/bin/g++ g++ /usr/bin/g++-8

echo "Installing Boost"
sudo apt-get install libboost-all-dev -y

echo "Installing Lib4L"
sudo apt-get install libv4l-dev libv4lconvert0 -y

echo "Installing GTKmm"
sudo apt-get install libgtkmm-3.0-dev -y

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
