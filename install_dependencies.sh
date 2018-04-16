#!/bin/bash
set -e

mkdir dependencies/
cd dependencies/

echo "Installing Boost"
sudo apt-get install libboost-all-dev

echo "Installing Lib4L"
sudo apt-get install libv4l-dev libv4lconvert0

echo "Installing GTKmm"
sudo apt-get install libgtkmm-3.0-dev

echo "Installing OpenCV"
git clone https://github.com/opencv/opencv.git
cd opencv/
mkdir build/
cd build/
cmake -D CMAKE_BUILD_TYPE=Release -D CMAKE_INSTALL_PREFIX=/usr/local ..
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

echo "All dependencies were installed. Run ./buildVSSS.sh to build"
