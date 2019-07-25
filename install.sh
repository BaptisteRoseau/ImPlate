#TODO: Dependencies for OpenCV

#Note: do not change anything
ROOT_DIR=$(pwd)
LPB_DIR=$ROOT_DIR/LPB

OPENCV_VERSION=3.4.5
OPENCV_INSTALL_DIR=$LPB_DIR/opencv     # Directory where OpenCV's librairies and headers will be installed
OPENALPR_INSTALL_DIR=$LPB_DIR/openalpr # Directory where OpenALPR's librairies and headers will be installed

#=================== OPENCV
cd $ROOT_DIR

## Getting OpenCV from source
wget https://github.com/opencv/opencv/archive/$OPENCV_VERSION.zip
unzip $OPENCV_VERSION.zip
rm -f $OPENCV_VERSION.zip

# Compiling and installing OpenCV
mkdir -p opencv-$OPENCV_VERSION/build
cd opencv-$OPENCV_VERSION/build
cmake -p -DCMAKE_BUILD_TYPE=RELEASE \
         -DCMAKE_INSTALL_PREFIX=$OPENCV_INSTALL_DIR \
         $ROOT_DIR/opencv-$OPENCV_VERSION/
make -j4
make -j4 install

#=================== OPEN ALPR
cd $ROOT_DIR

### Getting OpenAlpr
git clone https://github.com/sunfic/openalpr-opencv4 # Works with OpenCV 4.0.1 or higher
#git clone https://github.com/openalpr/openalpr.git # Works with OpenCV 2.4.8, not higher
cd openalpr-opencv4
mkdir -p build
cd build
cmake -p -DOpenCV_DIR=$OPENCV_INSTALL_DIR \
         -DOpenCV_VERSION=$OPENCV_VERSION \
         -DCMAKE_INSTALL_PREFIX=$OPENALPR_INSTALL_DIR \
         ../src/
make -j4
make -j4 install


#=================== $LPB_DIR
cd $LPB_DIR

## Building source
make install

# Testing on data
echo "Testing..."
(make test && echo "Testing success.\nInstallation complete.") || echo "Testing failed."
#PATH=$PATH:$LPB_DIR/build/blur
echo "Complete."