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
git clone -b 4.0.1 https://github.com/opencv/opencv.git
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

echo 'Installing simulator dependencies'
sudo apt install libglfw3 libglfw3-dev libglu1-mesa-dev freeglut3-dev -y

echo "Installing ROS"
sudo sh -c 'echo "deb http://packages.ros.org/ros/ubuntu $(lsb_release -sc) main" > /etc/apt/sources.list.d/ros-latest.list'
sudo apt-key adv --keyserver 'hkp://keyserver.ubuntu.com:80' --recv-key C1CF6E31E6BADE8868B172B4F42ED6FBAB17C654
sudo apt update
sudo apt install ros-melodic-desktop
sudo rosdep init
rosdep update
echo "source /opt/ros/melodic/setup.bash" >> ~/.bashrc
source ~/.bashrc
sudo apt install python-rosinstall python-rosinstall-generator python-wstool build-essential -y

echo "Installing vsss_msgs"
cd vsss_msgs_ws
VSSS_MSGS_LOCATION=/home/$USER/.vsss/vsss_msgs
catkin_make install -DCMAKE_INSTALL_PREFIX=$VSSS_MSGS_LOCATION
echo 'source /home/$USER/.vsss/vsss_msgs/setup.bash' >> ~/.bashrc
source ~/.bashrc

echo "All dependencies were installed. Run ./buildVSSS.sh to build the project."
