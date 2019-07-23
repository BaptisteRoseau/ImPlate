#TODO: Dependencies for OpenCV

ROOT=$(pwd)

# Getting OpenCV from source
wget https://github.com/opencv/opencv/archive/4.0.1.zip
unzip 4.0.1.zip
rm 4.0.1.zip
cd opencv-4.0.1/
mkdir build && cd build
cmake ..
make -j4

cd $ROOT

# Getting OpenAlpr for OpenCV4
git clone https://github.com/sunfic/openalpr-opencv4.git
cd openalpr-opencv4/
mkdir build && cd build
cmake ../src/ #TODO: Specifier le path to opencv
make -j4

cd $ROOT
mv openalpr-opencv4 alpr