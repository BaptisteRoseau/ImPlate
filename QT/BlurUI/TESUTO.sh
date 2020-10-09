rm -r ../../data_out
cp -r ../../data_bk/* ../../data/
../build-BlurUI-Desktop_Qt_5_15_1_GCC_64bit-Debug/BlurUI -s -v -r -i ../../data -o ../../data_out -l log.txt
