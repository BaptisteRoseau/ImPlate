mkdir -p build && cd build && rm -rf *
export PATH="/usr/local/Qt-5.15.2/bin:${PATH}"
export LD_LIBRARY_PATH="/usr/local/Qt-5.15.2/lib:${LD_LIBRARY_PATH}"
export INCLUDE_PATH="/usr/local/Qt-5.15.2/include:${INCLUDE_PATH}"
QT_PLUGIN_PATH="/usr/local/Qt-5.15.2/plugins/" cmake .. -DCMAKE_BUILD_TYPE=Release && make -j4 && make docs
./blur_gui -i ../../data -o ../../data_out/
