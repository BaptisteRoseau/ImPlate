ROOT_DIR=$(pwd)
LPB_DIR=$ROOT_DIR/LPB

OPENCV_VERSION=4.0.1


#=================== CUDA
cd $ROOT_DIR

#if ! [ -x "$(command -v cuda)" ]; then
#    echo "Donwloading cuda, this make take a few moment (1.7GB)"
#    wget https://developer.nvidia.com/compute/cuda/10.1/Prod/local_installers/cuda-repo-ubuntu1810-10-1-local-10.1.168-418.67_1.0-1_amd64.deb
#    sudo apt-key add /var/cuda-repo-10-1-local-10.1.168-418.67/7fa2af80.pub
#    sudo apt-get update
#    sudo apt-get install cuda
#fi


#=================== OPENCV
cd $ROOT_DIR

#Opencv dependencies
sudo apt-get install -y cmake
sudo apt-get install -y libgtk-3-dev
sudo apt-get install -y git

if ! [ -x "$(command -v opencv)" ]; then #FIXME: le test ne fonctionne pas
    ### Getting OpenCV from source
    wget https://github.com/opencv/opencv/archive/$OPENCV_VERSION.zip
    unzip $OPENCV_VERSION.zip
    rm -f $OPENCV_VERSION.zip

    # Compiling and installing OpenCV
    mkdir -p opencv-$OPENCV_VERSION/build
    cd opencv-$OPENCV_VERSION/build
    cmake -DCMAKE_BUILD_TYPE=Release       \
          -DENABLE_CXX11=ON                \
          -DBUILD_PERF_TESTS=OFF           \
          -DWITH_XINE=ON                   \
          -DBUILD_TESTS=OFF                \
          -DENABLE_PRECOMPILED_HEADERS=OFF \
          -DCMAKE_SKIP_RPATH=ON            \
          -DBUILD_WITH_DEBUG_INFO=OFF      \
          -Wno-dev  $ROOT_DIR/opencv-$OPENCV_VERSION/
    make -j4
    sudo make -j4 install
fi

#=================== OPEN ALPR
cd $ROOT_DIR

#OpenALPR dependencies
sudo apt-get install -y tesseract-ocr
sudo apt-get install -y libtesseract-dev
sudo apt-get install -y libcurl4-openssl-dev
sudo apt-get install -y liblog4cplus-dev

if ! [ -x "$(command -v alpr)" ]; then
    ### Getting OpenAlpr
    git clone https://github.com/sunfic/openalpr-opencv4 # Works with OpenCV 4.0.1 or higher
    #git clone https://github.com/openalpr/openalpr.git # Works with OpenCV 2.4.8 only
    cd openalpr-opencv4
    mkdir -p build
    cd build
    cmake ../src/
    make -j4
    sudo make install
fi

#=================== LICEN PLATE BLUR
cd $LPB_DIR

## Building source
make install

# Testing on data
echo "Testing..."
(make run && echo "Testing success.\nInstallation complete.") || echo "Testing failed."
#PATH=$PATH:$LPB_DIR/build/blur
echo "Complete."

#========= Cleaning
cd $ROOT_DIR

rm -rf openalpr-opencv4
rm -rf opencv-$OPENCV_VERSION
