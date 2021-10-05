FROM ubuntu:18.04

ENV DEBIAN_FRONTEND=noninteractive

WORKDIR /dependecies

# LIBRARIES
RUN apt-get update && \
    apt-get install -y \
        build-essential \
        cmake \
        git \
        libgtk2.0-dev \
        pkg-config \
        libavcodec-dev \
        libavformat-dev \
        libswscale-dev \
        software-properties-common \
        gdb

# GCC-10
RUN add-apt-repository ppa:ubuntu-toolchain-r/test -y && \
    apt-get update && \
    apt-get install gcc-10 g++-10 -y && \
    update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-7 700 --slave /usr/bin/g++ g++ /usr/bin/g++-7 && \
    update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-10 800 --slave /usr/bin/g++ g++ /usr/bin/g++-10

# OPENCV
RUN git clone --branch 4.0.1 --depth 1 \
        https://github.com/opencv/opencv.git &&\
    mkdir ./opencv/build &&\
    cd ./opencv/build &&\
    cmake -D CMAKE_BUILD_TYPE=Release \
        -D CMAKE_INSTALL_PREFIX=/usr/local .. &&\
    make -j$(nproc) 2> /dev/null &&\
    make install &&\
    cd /dependecies &&\
    rm -rf *

# Xbee
RUN git clone https://github.com/lopelope/libxbee3.git &&\
    cd ./libxbee3 &&\
    git remote update &&\
    git fetch &&\
    git checkout waitforack-needsfree-issues-fix &&\
    make configure &&\
    make -j$(nproc) &&\
    make install &&\
    cd /dependecies &&\
    rm -rf *

# VSSS Dependencies
RUN apt-get install -y \
        libboost-all-dev \
        libv4l-dev \
        libv4lconvert0 \
        libgtkmm-3.0-dev

RUN apt-get install -y wget autoconf automake libtool \
        curl make unzip libgoogle-glog-dev libevent-dev \
        python3-pip qt5-default libqt5opengl5-dev libgl1-mesa-dev \
        libglu1-mesa-dev libode-dev mesa-utils libgl1-mesa-glx libnvidia-gl-440 protobuf-compiler libprotoc-dev

# EVPP (TCP/UDP library)
RUN git clone --recurse-submodules https://github.com/Qihoo360/evpp.git &&\
    mkdir ./evpp/build &&\
    cd ./evpp/build &&\
    cmake .. && \
    make -j$(nproc) &&\
    make install &&\
    cd /dependecies &&\
    rm -rf *

# AI
RUN pip3 install --upgrade setuptools pip
RUN pip3 install scikit-build
RUN pip3 install gym tensorflow==1.14.0 opencv-python

RUN git clone https://github.com/BrunoBSM/stable-baselines &&\
    cd ./stable-baselines/ &&\
    pip3 install -e . &&\
    cd /dependecies &&\
    rm -rf *

# FIRASim
ENV QT_X11_NO_MITSHM=1
RUN mkdir -m 700 /tmp/runtime-root
ENV XDG_RUNTIME_DIR=/tmp/runtime-root

RUN git clone https://github.com/jpfeltracco/vartypes.git &&\
    mkdir ./vartypes/build &&\
    cd ./vartypes/build &&\
    cmake .. &&\
    make -j$(nproc) &&\
    make install &&\
    cd /dependecies &&\
    rm -rf *

WORKDIR /root/
RUN git clone https://github.com/VSSSLeague/FIRASim.git &&\
    mkdir ./FIRASim/build &&\
    cd ./FIRASim/build &&\
    cmake ..  &&\
    make -j$(nproc)

# Referee
WORKDIR /root/
RUN git clone https://github.com/VSSSLeague/VSSReferee.git &&\
    mkdir ./VSSReferee/build &&\
    cd ./VSSReferee/build &&\
    qmake .. &&\
    make -j$(nproc)

# VSSS for headless
WORKDIR /root/VSSS
COPY img /root/VSSS/img
COPY protobuf_msg /root/VSSS/protobuf_msg
COPY yellow_headless.json /root/VSSS/
COPY blue_headless.json /root/VSSS/
COPY python /root/VSSS/python
COPY pack-capture-gui /root/VSSS/pack-capture-gui
COPY pack-capture /root/VSSS/pack-capture
COPY CMakeLists.txt /root/VSSS/
COPY cc /root/VSSS/cc
RUN mkdir ./build &&\
    cd ./build &&\
    cmake .. &&\
    make -j$(nproc)

RUN rm -rf /dependecies
WORKDIR /root/src
