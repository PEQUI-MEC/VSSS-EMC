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
WORKDIR /dependecies
RUN git clone \
        --branch 4.0.1 \
        --depth 1 \
        https://github.com/opencv/opencv.git 
WORKDIR ./opencv/build
RUN cmake \
        -D CMAKE_BUILD_TYPE=Release \
        -D CMAKE_INSTALL_PREFIX=/usr/local ..
RUN make -j$(nproc) 2> /dev/null #not printing warnings or errors
RUN make install
WORKDIR /dependecies
RUN rm -rf *

# Xbee
WORKDIR /dependecies
RUN git clone https://github.com/lopelope/libxbee3.git
WORKDIR ./libxbee3
RUN git remote update
RUN git fetch
RUN git checkout waitforack-needsfree-issues-fix
RUN make configure
RUN make -j$(nproc)
RUN make install
WORKDIR /dependecies
RUN rm -r *

# VSSS Dependencies
RUN apt-get install -y \
        libboost-all-dev \
        libv4l-dev \
        libv4lconvert0 \
        libgtkmm-3.0-dev

RUN apt-get install -y wget autoconf automake libtool \
    curl make unzip libgoogle-glog-dev libevent-dev \
    python3-pip qt5-default libqt5opengl5-dev libgl1-mesa-dev \
    libglu1-mesa-dev libode-dev mesa-utils libgl1-mesa-glx libnvidia-gl-440

# Protobuf
WORKDIR /dependecies
RUN wget https://github.com/protocolbuffers/protobuf/releases/download/v3.13.0/protobuf-cpp-3.13.0.zip && \
    unzip protobuf-cpp-3.13.0.zip
WORKDIR ./protobuf-3.13.0
RUN ./autogen.sh && \
    ./configure
RUN make -j$(nproc) && \
    make install && \
    ldconfig
WORKDIR /dependecies
RUN rm -r *

# EVPP (TCP/UDP library)
WORKDIR /dependecies
RUN git clone --recurse-submodules https://github.com/Qihoo360/evpp.git
WORKDIR ./evpp/build
RUN cmake .. && \
    make -j$(nproc) && \
    make install
WORKDIR /dependecies
RUN rm -r *

# AI
RUN pip3 install --upgrade setuptools pip
RUN pip3 install scikit-build
RUN pip3 install gym tensorflow==1.14.0 opencv-python

WORKDIR /dependecies
RUN git clone https://github.com/BrunoBSM/stable-baselines
WORKDIR ./stable-baselines/
RUN pip3 install -e .
WORKDIR /
WORKDIR /dependecies
RUN rm -r *

# FIRASim
WORKDIR /dependecies
RUN git clone https://github.com/jpfeltracco/vartypes.git
WORKDIR ./vartypes/build
RUN cmake ..
RUN make -j$(nproc)
RUN make install
WORKDIR /dependecies
RUN rm -r *

WORKDIR /root/
RUN git clone https://github.com/VSSSLeague/FIRASim.git
WORKDIR ./FIRASim/build
RUN cmake ..
RUN make -j$(nproc)

# Referee
WORKDIR /root/
RUN git clone https://github.com/VSSSLeague/VSSReferee.git
WORKDIR ./VSSReferee/build
RUN qmake ..
RUN make -j$(nproc)

# VSSS for headless
WORKDIR /root/VSSS
COPY cc /root/VSSS/cc
COPY img /root/VSSS/img
COPY protobuf_msg /root/VSSS/protobuf_msg
COPY python /root/VSSS/python
COPY pack-capture-gui /root/VSSS/pack-capture-gui
COPY pack-capture /root/VSSS/pack-capture
COPY CMakeLists.txt /root/VSSS/
COPY yellow_headless.json /root/VSSS/
COPY blue_headless.json /root/VSSS/
WORKDIR ./build
RUN cmake ..
RUN make -j$(nproc)

RUN rm -rf /dependecies
WORKDIR /root/src
