# Licence Plate Blur

This project's purpose is to automatically blur license plates.

# Installation

## Requierement

All this requirement will ve installed by using `sudo install_sudo.sh`
- Tesseract OCR v3.0.4+ (https://github.com/tesseract-ocr/tesseract)
- OpenCV v4.0.1+ (http://opencv.org/)
- OpenALPR (https://github.com/openalpr/openalpr)
- OpenCV Dependencies (http://www.linuxfromscratch.org/blfs/view/svn/general/opencv.html)

Optional:
- CUDA (https://developer.nvidia.com/cuda-downloads)



## Installation Script

The easiest way to install the software if your have access to sudo is to run `sudo install_sudo.sh` (you need to have `apt` installed). It require an internet connection and may take several minutes. Please read carefully this script before launching it. Once it is done, you can add the path to the `blur` executable to your `PATH`.

For a faster runtime, install CUDA then recompile OpenCV and OpenALPR so it will use the GPU instead of the CPU for the Neural Networks. Note that an NVIDIA GTX 1XXX GPU or higher is required to use CUDA.
**Read carefully this README and the installation scripts before installing anything**, as you may have already some dependecies installed, and don't need to reinstall them. Feel free to modify the installation scripts if necessary. Note that OpenCV 4.0.1 or higher is requiered, and CUDA is highly recommended (install it **before** OpenCV or OpenALPR).

If you want to manually install it, OpenCV (and it's dependencies) and OpenALPR must be installed. You can see how to install it from scratch here: [http://www.linuxfromscratch.org/blfs/view/svn/general/opencv.html]. You can also see the requiered packages and the different steps into the `install_sudo.sh` script.

After the installation, the binary will be located at:

`LPB/build/blur`

By using the `sudo install_sudo.sh` script, this line will be automatically added to your `~/.bashrc`, allowing you to use `blur` command wherever you want.
The following line  `~/.bashrc`:
`PATH=$PATH:<path to>/LPB/build/blur`

A set of pictures are located in the [LPB/data](LPB/data), they are only used for testing during the first installation. Your are free to removes them after the installation is completed.

**Do not move the repository after installation**



## Build Documentation

If you wish to build the documentation, go into the LPB directory and run the command:

`make doc` or `doxygen Doxyfile`

The documenation will be located at [LPB/doc/html/index.html](LPB/doc/html/index.html). The details of the functions can be found on the headers files (*.h).
If the code needs to be recompiled, go into the [LPB](LPB) directory and run the command `make install`.



# How to use

Once installation is complete and path added to the Environment Variables, you can use the software with this command line:

`blur -i <path to picture or directory> -o <output directory> [args]`

Input and output path are requiered, and there are several options available:

Requiered argument:

- `-i` or `--input`:  The path to the input file or directory.
- `-o` or `--output`: The path to the output file or directory. Output file path is only available with a file as input. Directories will be created if they don't exist. It can be the same as `--input`.

Optional argument:

- `-h` or `--help`:           Displays this screen.
- `-l` or `--save-log`:       The path to a file where all the output logs will be saved. Will be created if doesn't exist.
- `-a` or `--out-name-addon`: The name addon for every blured picture (default: '_blured').
- `-t` or `--timeout`:        A timeout in seconds.
- `-p` or `--blur-power`:     The size of the square box used to make a blur effect (default: 70).
- `-v` or `--verbose`:        Whether or not information has to be displayed. This does not affect the logs.
- `-c` or `--counry`:         The country code of the car, to match the country's plate pattern. (default: "eu")

- `-s` or `--save-info`:      Whether or not plate information sould be saved as well.
- `-r` or `--respect-path`:   Whether or not the path of output blured picture has to be similar to their path in the input directory.

Note that removing verbose, save-info and save-log can save up to a bit less than 1/3 of the running time.



# Improvement

For the moment, the country code is set to "eu" (European cars) by default, but can be easily changed if necessary.
In order to improve the plate detection accuracy of ALPR, please refer to <http://doc.openalpr.com/opensource.html#training-ocr>.



# Documentation

Don't hesitate to check the code in [LPB/src/main.cpp](LPB/src/main.cpp) in order to be able to make little changes if necessary.

- OpenCV4: <https://docs.opencv.org/4.0.1/>
- OpenALPR: <http://doc.openalpr.com/opensource.html>
- This software: [LPB/doc/html/index.html](LPB/doc/html/index.html) after building documentation.
