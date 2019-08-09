#TODO: Dependencies for OpenCV

#Note: do not change anything
ROOT_DIR=$(pwd)
LPB_DIR=$ROOT_DIR/LPB

OPENCV_VERSION=4.0.1

#=================== OPENCV
cd $ROOT_DIR

### Getting OpenCV from source
wget https://github.com/opencv/opencv/archive/$OPENCV_VERSION.zip
unzip $OPENCV_VERSION.zip
rm -f $OPENCV_VERSION.zip

# Compiling and installing OpenCV
mkdir -p opencv-$OPENCV_VERSION/build
cd opencv-$OPENCV_VERSION/build
cmake $ROOT_DIR/opencv-$OPENCV_VERSION/
make -j4
sudo make -j4 install

#=================== OPEN ALPR
cd $ROOT_DIR

### Getting OpenAlpr
git clone https://github.com/sunfic/openalpr-opencv4 # Works with OpenCV 4.0.1 or higher
#git clone https://github.com/openalpr/openalpr.git # Works with OpenCV 2.4.8, not higher
cd openalpr-opencv4
mkdir -p build
cd build
cmake ../src/
make -j4
sudo make -j4 install



#=================== $LPB_DIR
cd $ROOT_DIR

## Building source
make install

# Testing on data
echo "Testing..."
(make test && echo "Testing success.\nInstallation complete.") || echo "Testing failed."
#PATH=$PATH:$LPB_DIR/build/blur
echo "Complete."