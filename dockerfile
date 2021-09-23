FROM ubuntu:18.04

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
        software-properties-common

# GCC-10
RUN add-apt-repository ppa:ubuntu-toolchain-r/test -y && \
    apt-get update && \
    apt-get install gcc-10 g++-10 -y && \
    update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-7 700 --slave /usr/bin/g++ g++ /usr/bin/g++-7 && \
    update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-10 800 --slave /usr/bin/g++ g++ /usr/bin/g++-10

# OPENCV
WORKDIR /opencv_workspace
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
RUN rm -r /opencv_workspace

# Xbee
WORKDIR /Xbee_workspace
RUN git clone https://github.com/lopelope/libxbee3.git
WORKDIR ./libxbee3
RUN git remote update
RUN git fetch
RUN git checkout waitforack-needsfree-issues-fix
RUN make configure
RUN make -j$(nproc)
RUN make install
RUN rm -r /Xbee_workspace

# VSSS
RUN apt-get install -y \
        libboost-all-dev \
        libv4l-dev \
        libv4lconvert0 \
        libgtkmm-3.0-dev

RUN apt-get install -y wget autoconf automake libtool \
    curl make unzip libgoogle-glog-dev libevent-dev \
    python3-pip qt5-default libqt5opengl5-dev libgl1-mesa-dev \
    libglu1-mesa-dev libode-dev -y

WORKDIR /protobuf_workspace
RUN wget https://github.com/protocolbuffers/protobuf/releases/download/v3.13.0/protobuf-cpp-3.13.0.zip && \
    unzip protobuf-cpp-3.13.0.zip
WORKDIR ./protobuf-3.13.0
RUN ./autogen.sh && \
    ./configure
RUN make -j$(nproc) && \
    make install &&\
    ldconfig
RUN rm -r /protobuf_workspace

WORKDIR /evpp_workspace
RUN git clone --recurse-submodules https://github.com/Qihoo360/evpp.git
WORKDIR ./evpp
WORKDIR ./build
RUN cmake .. && \
    make -j$(nproc) && \
    make install
RUN rm -r /evpp_workspace

RUN pip3 install --upgrade setuptools pip
RUN pip3 install scikit-build
RUN pip3 install gym tensorflow==1.14.0 opencv-python

WORKDIR /stable_baselines_workspace
RUN git clone https://github.com/BrunoBSM/stable-baselines
WORKDIR ./stable-baselines/
RUN pip3 install -e .
RUN rm -r /stable_baselines_workspace

COPY cc /cc
COPY img /img
COPY protobuf_msg /protobuf_msg
COPY python /python
COPY pack-capture-gui /pack-capture-gui
COPY pack-capture /pack-capture
COPY CMakeLists.txt /
WORKDIR /build
RUN cmake ..
RUN make -j$(nproc)
WORKDIR /
