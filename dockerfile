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

# GCC-8
RUN add-apt-repository ppa:ubuntu-toolchain-r/test -y && \
    apt-get update && \
    apt-get install gcc-8 g++-8 -y && \
    update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-7 700 --slave /usr/bin/g++ g++ /usr/bin/g++-7 && \
    update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-8 800 --slave /usr/bin/g++ g++ /usr/bin/g++-8

# OPENCV
WORKDIR /opencv_workspace
RUN git clone \
        --branch 3.4.1 \
        --depth 1 \
        https://github.com/opencv/opencv.git 
WORKDIR ./opencv/build
RUN cmake \
        -D CMAKE_BUILD_TYPE=Release \
        -D CMAKE_INSTALL_PREFIX=/usr/local ..
RUN make -j7
RUN make install
RUN rm -r /opencv_workspace

# Xbee
WORKDIR /Xbee_workspace
RUN git clone \
        --depth 1 \
        https://github.com/attie/libxbee3.git
WORKDIR ./libxbee3
RUN make configure
RUN make -j7
RUN make install
RUN rm -r /Xbee_workspace

# VSSS
WORKDIR /VSS_workspace
RUN apt-get install -y \
        libboost-all-dev \
        libv4l-dev \
        libv4lconvert0 \
        libgtkmm-3.0-dev

COPY . .
RUN ./buildVSSS.sh
CMD ./runVSSS.sh
