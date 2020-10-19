rm -rf data_in data_out
cp -r data data_in
program/build/blur_gui -i data_in -o data_out -l log.txt -b 150 -v -s -p -r
echo "Done"
