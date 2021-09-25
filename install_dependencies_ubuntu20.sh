#!/bin/bash
set -e

mkdir dependencies/
cd dependencies/

echo "Installing compilation tools"
sudo apt update
sudo apt-get install build-essential cmake -y

echo "Installing gcc-10"
sudo apt-get install gcc-10 g++-10 -y
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-9 700 --slave /usr/bin/g++ g++ /usr/bin/g++-9
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-10 800 --slave /usr/bin/g++ g++ /usr/bin/g++-10

echo "Installing Boost"
sudo apt-get install libboost-all-dev -y

echo "Installing Lib4L"
sudo apt-get install libv4l-dev libv4lconvert0 -y

echo "Installing GTKmm"
sudo apt-get install libgtkmm-3.0-dev -y

echo "Installing OpenCV"
git clone -b 4.0.1 https://github.com/opencv/opencv.git
cd opencv/
mkdir build/
cd build/
cmake -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local -D WITH_CUDA=ON -D WITH_CUBLAS=ON -D BUILD_PERF_TESTS=OFF -D BUILD_TESTS=OFF -D BUILD_opencv_java=OFF -D CUDA_GENERATION=Auto -D CUDA_NVCC_FLAGS="-D_FORCE_INLINES" ..
make -j$(nproc)
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

echo "Installing simulator dependencies"
./simu_dependencies.sh

echo "All dependencies were installed. Run ./buildVSSS.sh to build the project."
