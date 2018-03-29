FROM ubuntu:17.10

# OPENCV
RUN apt-get update && \
    apt-get install -y \
        build-essential \
        cmake \
        git \
        libgtk2.0-dev \
        pkg-config \
        libavcodec-dev \
        libavformat-dev \
        libswscale-dev

WORKDIR /opencv_workspace
RUN git clone \
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
RUN clone \
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
RUN ./make.sh
CMD ./runVSSS.sh
