#TODO: Dependencies for OpenCV

OPENCV_VERSION=3.4.5
ROOT_DIR=$(pwd)

# OPENCV

## Getting OpenCV from source
#wget https://github.com/opencv/opencv/archive/$OPENCV_VERSION.zip
#unzip $OPENCV_VERSION.zip
#rm -f $OPENCV_VERSION.zip
#
## Compiling OpenCV
#mkdir -p opencv-$OPENCV_VERSION/build_opencv
#cd opencv-$OPENCV_VERSION/build_opencv
#echo $ROOT_DIR/opencv-$OPENCV_VERSION/
#cmake -p -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=$ROOT_DIR/build_opencv/opencv3_install $ROOT_DIR/opencv-$OPENCV_VERSION/
#make -j4

## Retrieving librairies and includes
#echo "Copying librairies and headers for opencv..."
#cd $ROOT_DIR
#mkdir -p  LPB/lib
#mkdir -p  LPB/lib/opencv
#mkdir -p  LPB/include
#mkdir -p  LPB/include/opencv
#cp opencv-$OPENCV_VERSION/build_opencv/lib/*.so opencv-$OPENCV_VERSION/build_opencv/lib/*.a LPB/lib/opencv/
#cp -r opencv-$OPENCV_VERSION/modules/* LPB/include/opencv/
#rm -f $(find LPB/include/opencv/ -type f -not -iname *.h*)
#rm -rf $(find LPB/include/opencv/ -type d -empty)

# OPEN ALPR

## Getting OpenAlpr
git clone https://github.com/openalpr/openalpr
cd openalpr/
mkdir -p build && cd build
cmake ../src/ #TODO: Specifier le path to opencv
make -j4


## ## Retrieving librairies and includes
echo "Copying librairies and headers for openalpr..."
cd $ROOT_DIR
mkdir -p  LPB/lib
mkdir -p  LPB/lib/openalpr
mkdir -p  LPB/include
mkdir -p  LPB/include/openalpr