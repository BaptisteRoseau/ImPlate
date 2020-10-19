FROM ubuntu:latest

# Set the working directory.
ENV WORKDIR_DIR=/usr/app
WORKDIR $WORKDIR_DIR

# Getting sources directory from host
COPY Dockerfile $WORKDIR_DIR
COPY program $WORKDIR_DIR/program

ENV ROOT_DIR=/opt/tmp_install
RUN mkdir -p $ROOT_DIR $WORKDIR_DIR
ENV OPENCV_VERSION=4.5.0
#ENV OPENCV_VERSION=4.2.0

#=================== OPENCV
#Opencv dependencies
RUN ln -s /usr/share/zoneinfo/Europe/London /etc/localtime && \
    DEBIAN_FRONTEND=noninteractive apt-get update
# DEBIAN_FRONTEND=noninteractive apt-get upgrade -y
RUN apt-get install -y \
        cmake \
        libgtk-3-dev \
        g++ \
        git \
        unzip \
        wget

### Getting OpenCV from source
RUN wget https://github.com/opencv/opencv/archive/$OPENCV_VERSION.zip && \
    unzip $OPENCV_VERSION.zip && \
    rm -f $OPENCV_VERSION.zip

# Compiling and installing OpenCV
RUN mkdir -p opencv-$OPENCV_VERSION/build && \
    cd opencv-$OPENCV_VERSION/build && \
    cmake -DCMAKE_BUILD_TYPE=Release     \
          -DENABLE_CXX11=ON                \
          -DBUILD_PERF_TESTS=OFF           \
          -DWITH_XINE=ON                   \
          -DBUILD_TESTS=OFF                \
          -DENABLE_PRECOMPILED_HEADERS=OFF \
          -DCMAKE_SKIP_RPATH=ON            \
          -DBUILD_WITH_DEBUG_INFO=OFF      \
          -DBUILD_EXAMPLES=OFF             \
          -Wno-dev  .. && \
    make -j4 && \
    make -j4 install

#-DCMAKE_CXX_COMPILER=g++
#=================== OPEN ALPR
#OpenALPR dependencies
RUN apt-get install -y \
        tesseract-ocr \
        libtesseract-dev \
        libcurl4-openssl-dev \
        liblog4cplus-dev 

### Getting OpenAlpr
RUN git clone https://github.com/sunfic/openalpr-opencv4 # Works with OpenCV 4.0.1 or higher
RUN cd openalpr-opencv4 && \
    mkdir -p build && \
    cd build && \
    cmake ../src/ && \
    make -j4 && \
    make install

#=================== QT 5.15
RUN git clone git://code.qt.io/qt/qt5.git
RUN cd qt5 && ./init-repository \
     && mkdir -p build && cd build && \
    ../configure -release -opensource -nomake tests -nomake examples -confirm-license && \
    make -j4 && \
    make install
#-prefix $PWD/qtbase

ENV PATH="/usr/local/Qt-5.15.2/bin:${PATH}"
ENV LD_LIBRARY_PATH="/usr/local/Qt-5.15.2/lib:${LD_LIBRARY_PATH}"
ENV INCLUDE_PATH="/usr/local/Qt-5.15.2/include:${INCLUDE_PATH}"

RUN echo "PATH=/usr/local/Qt-%VERSION%/bin:\$PATH" >> ~/.profile
RUN echo "export PATH" >> ~/.profile


#=================== LICENCE PLATE BLUR

RUN cd program && ./build.sh

#========= Cleaning and setup environment
#RUN rm -rf $ROOT_DIR



#========= Docker Commands




# https://medium.com/@SaravSun/running-gui-applications-inside-docker-containers-83d65c0db110
# For GUI, use : docker run --net=host --env="DISPLAY" --volume="$HOME/.Xauthority:/root/.Xauthority:rw" gui-app






# BACKUP
# Installing dependencies
# OpenCV
#RUN apt-get update && \
#    apt-get install -y \
#        build-essential \
#        cmake \
#        unzip \
#        pkg-config \
#        libjpeg-dev \
#        libpng-dev \
#        libtiff-dev \
#        libavcodec-dev  \
#        libavformat-dev  \
#        libswscale-dev \
#        libv4l-dev \
#        libxvidcore-dev \
#        libx264-dev \
#        libgtk-3-dev \
#        libatlas-base-dev \
#        gfortran 