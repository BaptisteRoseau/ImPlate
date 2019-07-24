#TODO: Dependencies for OpenCV

OPENCV_VERSION=3.4.5
ROOT_DIR=$(pwd)

# OPENCV

# Getting OpenCV from source
wget https://github.com/opencv/opencv/archive/$OPENCV_VERSION.zip
unzip $OPENCV_VERSION.zip
rm -f $OPENCV_VERSION.zip

# Compiling OpenCV
mkdir -p opencv-$OPENCV_VERSION/build_opencv
cd opencv-$OPENCV_VERSION/build_opencv
echo $ROOT_DIR/opencv-$OPENCV_VERSION/
cmake -p -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=$ROOT_DIR/opencv3_install $ROOT_DIR/opencv-$OPENCV_VERSION/
make -j4

# Retrieving shared librairies and includes


cd $ROOT_DIR

# OPEN ALPR

#
## Getting OpenAlpr
#git clone https://github.com/openalpr/openalpr
#cd openalpr/
#mkdir -p build && cd build
#cmake ../src/ #TODO: Specifier le path to opencv
#make -j4
#
#cd $ROOT_DIR
#mv openalpr alpr