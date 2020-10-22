set -e # Exit on error
cd $(dirname $0) && ./clean.sh && cp -r data data_in
program/build/blur -i data_in -o data_out -l log.txt -b 150 -v -s -p -r
./clean.sh && cp -r data data_in
program/build/blur_gui -i data_in -o data_out -l log.txt -b 150 -v -s -p -r
echo "Test success"
