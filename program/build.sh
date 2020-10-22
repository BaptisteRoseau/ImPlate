set -e # Exit on error
cd $(dirname $0) && mkdir -p build && cd build && rm -rf *
cmake .. -DCMAKE_BUILD_TYPE=Release && make -j4 && make docs
cp -r docs/* ../doc
