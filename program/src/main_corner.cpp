#include "utils.h"
#include "blur.h"
#include "options.h"
#include "gopt/gopt.h"
#include "config.h"

#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/core/mat.hpp>

#include <filesystem>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/io.h>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <stack>
#include <limits.h>
#include <gtk/gtk.h>

using namespace std;
using namespace cv;
namespace fs = filesystem;

#define DFLT_OUTPUT_ADDON "_blured"
#define DFLT_BLUR 70
#define BUFFSIZE 200

bool verbose; /// Whether or not information should be displayed
bool save_log; /// Whether or not logs should be saved (default: false)
ofstream log_ostream; /// Stream to the file where the logs will be saved

// Used by CallBackFunc
bool finished = false;
Mat ori, img;
vector<Point> vertices;
vector<vector<Point> > corners;

void draw_lines(){
    img = ori.clone();
    img.at<Vec3b>(vertices[0].x,vertices[0].y) = Vec3b(255,0,0);
    size_t vsize = vertices.size();
    size_t i;
    for (i = 1; i < vsize; i++){
        line(img, vertices[i-1], vertices[i], Scalar(0,0,0));
    }
    if (!vertices.empty() && vsize % 4 == 0){
        line(img, vertices[0], vertices[vsize-1], Scalar(0,0,0));
    }
}

void CallBackFunc(int event,int x,int y,int flags,void* userdata){
   if(event == EVENT_RBUTTONDOWN){
        DISPLAY("Right mouse button clicked at (" << x << ", " << y << ")");
        if (!vertices.empty()){
            vertices.pop_back();
        }
        draw_lines();
        return;
   }
   if(event == EVENT_LBUTTONDOWN){
        DISPLAY("Left mouse button clicked at (" << x << ", " << y << ")");
        vertices.push_back(Point(x,y));
        draw_lines();
        if (!vertices.empty() && vertices.size() % 4 == 0){
            corners.push_back(vertices);
            vertices.clear();
            finished = true;
        }
        return;
   }
}

int main_loop(const char* in_path, const char* out_path,
			const char *output_name_addon,
			const double timeout,
			const unsigned int blur_filter_size,
			const bool respect_input_path,
			const char *log_file){
    
	// Retrieving picture file paths into stack_files
	stack<string> *stack_files = list_files(in_path);
	if (stack_files == NULL){
		//Don't use DISPLAY_ERR 
		// you want to see this message even without verbose
		cerr << "\033[1;31mERROR:\033[0m Couldn't open any file.\n";
		exit(EXIT_FAILURE);
	}

	// Opening log file if necessary
	if (save_log){
		log_ostream.open(log_file);
		if (!log_ostream){
			cerr << "\033[1;31mERROR:\033[0m Couldn't open " << log_file << endl;
			exit(EXIT_FAILURE);
		}
	}

	// Building output directory if it doesn't exists, and if input is a directory
	if (!fs::directory_entry(in_path).is_regular_file()){
		if (!build_dir(out_path)){
			cerr << "\033[1;31mERROR:\033[0m Couldn't build output directory.\n";
			exit(EXIT_FAILURE);
		}
	}

	// Parameters initialisation
	stack<string> failed_pictures = stack<string>();
	Mat picture = Mat();
	Mat blured  = Mat();
	string filepath, filename, fileext, savedir, file_out_path;
	int error;
	unsigned int loop_idx = 0;
	unsigned int nb_files = stack_files->size();
	double _timeout = timeout == 0 ? DBL_MAX : timeout; 
	double t0 = time(NULL);


    while (!stack_files->empty()){
        // Reseting global variables
        destroyAllWindows();
        finished=false;
        vertices.clear();
        corners.clear();

        // Getting file path informations
		filepath = stack_next(stack_files);
		filename = get_filename(filepath);
		fileext  = get_file_extension(filepath);
		loop_idx++;

		// Displaying script advancement
		DISPLAY("\nPicture: " << loop_idx << " out of " << nb_files
			 << " (" << (float) 100*loop_idx/nb_files << " %) : " << filename)

		// Opening picture
		picture = open_picture(filepath);
		if (picture.empty()){
			DISPLAY_ERR("Couldn't open " << filepath);
			failed_pictures.push(filepath);
			continue;
		}

        //======================= MAIN DISPLAY FOR THIS PICTURE

        // Read image from file 
        ori = imread(filepath);

        // Check it loaded
        if(ori.empty()){ 
            DISPLAY_ERR("Couldn't load " << filepath)
            continue;
        }

        // Copying original image into buffer image
        img = ori.clone();

        //Create a window
        namedWindow("ImageDisplay", 1);

        // Register a mouse callback
        setMouseCallback("ImageDisplay", CallBackFunc, NULL);

        // Main loop
        //(TODO: 1 loop pour blur plusieurs zones)

        // Bluring loop
        while(!finished){
            imshow("ImageDisplay", img);
            waitKey(50);
        }

        // Bluring a copy of the initial picture
        blured = picture.clone();
        error = 0;
        for (auto&& corn: corners){
            error += _max(blur(picture, blured, corn, blur_filter_size), error);
        }
        if (error){
            DISPLAY_ERR("Couldn't blur" << error << " times out of " << corners.size() << " on " << filename);
            failed_pictures.push(filepath);
            continue;
        }

        // Show results
        destroyAllWindows();
        namedWindow("Result", 1);
        imshow("Result", blured);
        waitKey(3000);

        //======================= END MAIN DISPLAY

		// If input is a file, output will be directly out_path, not a directory
		if (fs::directory_entry(in_path).is_regular_file()){
			if (build_directories(out_path)){
				if (fs::path(out_path).has_extension()){
					// Saving file as in given
					savedir = (string) fs::path(out_path).parent_path();
					save_picture(blured, out_path);
				} else {
					// Saving file into the directory given
					savedir = out_path;
					save_picture(blured, out_path, filename+output_name_addon+fileext);
				}
			} else {
				continue;
			}
		// If input is a directory, 
		} else {
			// Selecting output directory and building requiered directories
			savedir = select_output_dir(out_path, in_path, filepath, respect_input_path);
			if (!build_directories(savedir)){
				continue;
			}
			// Writing blured picture into the directory
			save_picture(blured, savedir, filename+output_name_addon+fileext);
		}
		
		// Cleaning memory and exiting program if _timeout is reached
		if (difftime(time(NULL), t0) > _timeout){			
			delete stack_files;
			DISPLAY("Timeout reached.");
			return EXIT_SUCCESS;
		}
    }

    DISPLAY("\nExited successfully (" << difftime(time(NULL), t0) << " seconds).")

    // Cleaning memory
	delete stack_files;

    return EXIT_SUCCESS;
}

//===========================================================================================

void usage(char* name){
	cout <<"\
Usage: " << name << " -i <path to picture or directory>\
-o <output directory>\n\
Type -h or --help for more details.\n";
}

int main(int argc, char** argv){
    if (argc == 1){
		usage(argv[0]);
		return -1;
	}

    // Global variable initialization
	save_log = false;
	verbose  = false;
    finished = false;

	// Argument declaration and defaul value
	char* in_path = new char[BUFFSIZE];
	char* out_path = new char[BUFFSIZE];
	char *output_name_addon = new char[BUFFSIZE];
	strcpy(output_name_addon, DFLT_OUTPUT_ADDON);
	double timeout = 0;
	unsigned int blur_filter_size = DFLT_BLUR;
	bool respect_input_path = false;
	char *log_file = new char[BUFFSIZE];
	char *country = new char[BUFFSIZE];
	strcpy(country, "eu");
	bool save_plate_info = false;
	char *plate_info_save_path = NULL; // Dynamically allocated if argument provided
	bool blur_only = false;
	char *blur_only_location = new char[BUFFSIZE];
	bool replace_input_file = false;

	// Parsing command line
	parse_argv(argv, in_path, out_path,
			   output_name_addon,
			   timeout,
			   blur_filter_size,
			   verbose,
			   respect_input_path,
			   save_log,
			   log_file,
			   country,
			   save_plate_info,
			   plate_info_save_path,
			   blur_only,
			   blur_only_location,
			   replace_input_file);

	// Executing main process
	int ret = main_loop(in_path, out_path,
					  output_name_addon,
					  timeout,
					  blur_filter_size,
					  respect_input_path,
					  log_file);

	delete[] in_path;
	delete[] out_path;
	delete[] log_file;
	delete[] output_name_addon;
	delete[] country;
	delete[] blur_only_location;
	if (plate_info_save_path != NULL) delete[] plate_info_save_path;

    return ret;
    //return ret;
}
