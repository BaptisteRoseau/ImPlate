# Licence Plate Blur

This project's purpose is to automatically blur license plates.

# Installation

In order to use the software, OpenCV (and it's dependencies) and OpenALPR must be installed.
For a faster runtime, install CUDA and recompile OpenCV and OpenALPR so it will use the GPU instead of the CPU for the Neural Networks.
Note that an NVIDIA GTX 1XXX GPU or higher is required to use CUDA.

An installation script `install.sh` is given in the present directory. What it basically does is:

- Download and install OpenCV from source
- Download and install OpenALPR from source
- Copy the requiered source code, headers and precompiled librairies into the software's directory.
- Compile the software source code

The installation may take several minutes, and an internet connection is requiered.
After runing this script, OpenCV and OpenALPR source directories will remain, but you can freely remove them.
Their installation path (for headers and librairies) is located at [LPB/openalpr](LPB/openalpr) and [LPB/opencv](LPB/opencv) if you plan on using it's features for another project.

After the installation, the binary will be located at:

`LPB/build/blur`

You can add this to your PATH Environment Variable in your `.profile` home directory to use it more conveniently:

`PATH=$PATH:<path to>/LPB/build/blur`

If you wish to build the documentation, got into the LPB directory and run the command:

`make doc` or `doxygen Doxyfile`

The documenation will be located at [LPB/doc/html/index.html](LPB/doc/html/index.html). The details of the functions can be found on the headers files (*.h).
If the code needs to be recompiled, go into the [LPB](LPB) directory and run the command `make`. If you need to recompile OpenCV or OpenALPR, you can run `install.sh`, and comment the

# How to use

Once installation is complete and path added to the Environment Variables, you can use the software with this command line:

`blur -i <path to picture or directory> -o <output directory> [args]`

Input and output path are requiered, and there are several options available:

Requiered argument:

- `-i` or `--input`:  The path to the input file or directory.
- `-o` or `--output`: The path to the output directory. Will be created if doesn't exist.

Optional argument:

- `-h` or `--help`:           Displays this screen.
- `-l` or `--save-log`:       The path to a file where all the output logs will be saved. Will be created if doesn't exist.
- `-a` or `--out-name-addon`: The name addon for every blured picture (default: '_blured').
- `-t` or `--timeout`:        A timeout in seconds.
- `-p` or `--blur-power`:     The size of the square box used to make a blur effect (default: 70).
- `-v` or `--verbose`:        Whether or not information has to be displayed. This does not affect the logs.
- `-r` or `--respect-path`:   Whether or not the path of output blured picture has to be similar to their path in the input directory.(Not working yet)
- `-c` or `--counry`:         The country code of the car, to match the country's plate pattern. (default: "eu")
- `-s` or `--save-info`:      Whether or not plate information sould be saved as well.

# Improvement

For the moment, the country code is set to "eu" (European cars) by default, but can be easily changed if necessary.
In order to improve the plate detection accuracy of ALPR, please refer to <http://doc.openalpr.com/opensource.html#training-ocr>.

# Documentation

Don't hesitate to check the code in [LPB/src/main.cpp](LPB/src/main.cpp) in order to be able to make little changes if necessary.

- OpenCV3: <https://docs.opencv.org/3.4.5/>
- OpenALPR: <http://doc.openalpr.com/opensource.html>
- This software: [LPB/doc/html/index.html](LPB/doc/html/index.html) after building documentation.

# Requierement

- Tesseract OCR v3.0.4 (https://github.com/tesseract-ocr/tesseract)
- OpenCV v2.4.8+ (http://opencv.org/)
- OpenALPR (https://github.com/openalpr/openalpr)

Optional:
- CUDA (https://developer.nvidia.com/cuda-downloads)
