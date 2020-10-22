# Licence Plate Blur

This project's purpose is to automatically or manually blur license plates.
It comes with a non-GUI and a GUI executables having the same behavior, input and arguments.
If the non-GUI does not detect every plates, you can then blur them manually using the GUI executable.

# Installation

## Requirement

- g++ 8.3+
- CMake 3.5+
- OpenCV v4.0.1+ (http://opencv.org/)
- OpenCV Dependencies (http://www.linuxfromscratch.org/blfs/view/svn/general/opencv.html)
- OpenALPR (https://github.com/openalpr/openalpr)
- Tesseract OCR v3.0.4+ (https://github.com/tesseract-ocr/tesseract)
- Qt 5.14.2+ (https://www.qt.io/download)


## Installation Script

After making sure all dependencies are installed, run [program/build.sh](program/build.sh).
It will build the documentation and following executables:
- [program/build/blur](program/build/blur): The non-GUI blur executable
- [program/build/blur_gui](program/build/blur_gui): The GUI blur executable

The documentation will be located at [program/doc/html/index.html](program/doc/html/index.html) and can be opened with a web browser.
Details about the functions are given in the headers files (*.h).

To make sure the installation has been done successfully, you can run [test.sh](test.sh),
it will run the non-GUI and then the GUI program on the test data given in [data](data),
you can then clean the generated files using [clean.sh](clean.sh).

## Container

To build the Docker container given with this software,
install Docker and then run `docker build .` where the Dockerfile is located.


# How to use

Once installation is complete, you can run this software using:

`blur -i <path to picture or directory> -o <output directory> [args]`

Usage is exactly the same for `blur` and `blur_gui`.
Input and output path are always required, and there are several options available.


Required argument:

- `-i` or `--input`:  The path to the input file or directory.
- `-o` or `--output`: The path to the output file or directory. Output file path is only available with a file as input. Directories will be created if they don't exist. It can be the same as `--input`.

Optional argument:

- `-h` or `--help`:           Displays this screen.
- `-l` or `--save-log`:       The path to a file where all the output logs will be saved. Will be created if doesn't exist.
- `-a` or `--out-name-addon`: The name addon for every blurred picture (default: '_blurred').
- `-t` or `--timeout`:        A timeout in seconds.
- `-b` or `--blur-power`:     The size of the square box used to make a blur effect (default: 70).
- `-v` or `--verbose`:        Whether or not information has to be displayed. This does not affect the logs.
- `-c` or `--country`:         The country code of the car, to match the country's plate pattern. (default: "eu")
- `-s` or `--save-info`:      Whether or not plate information should be saved as well.
- `-p` or `--respect-path`:   Whether or not the path of output blurred picture has to be similar to their path in the input directory. Note that removing verbose, save-info and save-log can save up to a bit less than 1/3 of the running time.
- `-r` or `--rename`:         Rename input images with _backup and replace it with the blurred image.

For the moment, the country code is set to "eu" (European cars) by default, but can be easily changed if necessary.
In order to improve the plate detection accuracy of ALPR, please refer to <http://doc.openalpr.com/opensource.html#training-ocr>.

## Using a container

If you wish to run from the container, use the following commands:

TODO when Dockerfile is fixed.

In order to display the GUI of `blur_gui`, you need to run the container with
`--net=host --env="DISPLAY" --volume="$HOME/.Xauthority:/root/.Xauthority:rw"` options.



# Documentation

Don't hesitate to check the code in [program/src/blur/main.cpp](program/src/blur/main.cpp) in order to be able to make little changes if necessary.

- OpenCV4: <https://docs.opencv.org/4.0.1/>
- OpenALPR: <http://doc.openalpr.com/opensource.html>
- This software: [program/doc/html/index.html](program/doc/html/index.html) after building documentation.
