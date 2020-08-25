#!/bin/bash
set -e

mkdir dependencies/
cd dependencies/

echo "Installing dependencies"
sudo apt install wget autoconf automake libtool curl make unzip libgoogle-glog-dev libevent-dev python3-pip -y

echo "Installing protobuf"
wget https://github.com/protocolbuffers/protobuf/releases/download/v3.13.0/protobuf-cpp-3.13.0.zip
unzip protobuf-cpp-3.13.0.zip
cd protobuf-3.13.0
git submodule update --init --recursive
./autogen.sh
./configure
make -j$(nproc)
sudo make install
sudo ldconfig
cd ../

echo "Installing EVPP"
git clone --recurse-submodules https://github.com/Qihoo360/evpp.git
cd evpp/
mkdir build
cd build
cmake ..
make -j$(nproc)
sudo make install
cd ../../

echo "Installing python dependencies"
pip3 install scikit-build
pip3 install gym tensorflow==1.14.0 opencv-python
git clone https://github.com/BrunoBSM/stable-baselines
cd stable-baselines/
pip3 install -e .
cd ../

cd ../
rm -rf dependencies/

echo "All simulator dependencies were installed." 
