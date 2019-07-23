#include "utils.h"
#include "blur.h"
#include "options.h"
#include "gopt/gopt.h"

#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>
#include "../alpr/src/openalpr/alpr.h"

#include <filesystem>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/io.h>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <stack>
#include <limits.h>

using namespace std;
using namespace cv;
using namespace alpr;

namespace fs = filesystem;

#define DFLT_OUTPUT_ADDON "_blured"
#define DFLT_BLUR 70
#define BUFFSIZE 200

bool verbose;
bool save_log;
ofstream log_ostream; // To make log-file available everywhere

/* TODO:
	- Fix Stack Smashing
	- Find 4 corners
	- Fix "respect original path"
	- Better blur (or use OpenCV's blur)
	- Cmake
	- ajouter option "pays", "sauvegarder detections", "numero de plaque"
	- Ajouter dans la doc les options dispo pour le pays de la plaque

	Pour OpenCV et OpenALPR: les compiler à part, et linker directement les .so

 */




void plateCorners(const vector<AlprPlateResult> &results,
				 vector<vector<Point> > &corners,
				 vector<string> &numbers){
	size_t i, j;
	cout << results.size() << endl;
	for (i = 0; i < results.size(); i++){
		for (j = 0; j < 4; j++){
			corners[i][j].x = results[i].plate_points[j].x;
			corners[i][j].y = results[i].plate_points[j].y;
		}
		numbers[i] = results[i].bestPlate.characters;
	}
}

int process(const char* in_path, const char* out_dir,
			const char *output_name_addon,
			const double timeout,
			const unsigned int blur_filter_size,
			const bool respect_input_path,
			const char *log_file,
			const char *country){

	// Retrieving video file paths into stack_files
	stack<string> *stack_files = list_files(in_path);

	// Building output directory if it doesn't exists
	if (stack_files == NULL || build_dir(out_dir)){
		exit(EXIT_FAILURE);
	}

	// Opening log file if necessary
	if (save_log){
		log_ostream.open(log_file);
		if (!log_ostream){
			DISPLAY_ERR(format("Couldn't open %s", log_file));
			exit(EXIT_FAILURE);
		}
	}
	// Parameters initialisation
	stack<string> failed_pictures = stack<string>();
	Mat picture;
	Mat blured;
	string filepath, filename, fileext, savedir, file_out_dir;
	double _timeout = timeout == 0 ? DBL_MAX : timeout; 
	unsigned int loop_idx = 0;
	unsigned int nb_files = stack_files->size();
	double t0 = time(NULL);
	int error;

	// Main loop
	while (!stack_files->empty()){
		// Getting file path informations
		filepath = stack_next(stack_files);
		filename = get_filename(filepath);
		fileext  = get_file_extension(filepath); //TODO
		loop_idx++;

		// Displaying script advancement
		DISPLAY(format("\nPicture: %u out of %u (%.2f %%): ",
		 		loop_idx, nb_files, (double) 100*loop_idx/nb_files) << filename)

		// Opening picture
		picture = open_picture(filepath);
		if (picture.empty()){
			DISPLAY_ERR("Couldn't open " << filepath);
			failed_pictures.push(filename);
			continue;
		}

		/* ==== PLATE DETECTION AND BLUR BEGIN ==== */

		//TODO: s'il y a plusieurs voitures, toutes les flouter
		//TODO: Trouve les 4 coins via une api et les sauvegader dans un fichier

		// ALPR buffers variables
		vector<vector<Point> > corners = vector<vector<Point> >();
		vector<string> numbers = vector<string>();
		
		// Getting picture's plate informations
		vector<AlprPlateResult> results = Alpr(country).recognize(filepath).plates;
		plateCorners(results, corners, numbers);
		if (corners.size() == 0){
			DISPLAY_ERR("No plate detected on " << filename);
			picture.release();
			failed_pictures.push(filename);
			continue;
		}

		//vector<Point> corners = {{238,490}, {821, 318}, {857, 585}, {330, 786}}; //IMG
		//vector<Point> corners = {{173,507}, {338, 526}, {338, 567}, {172, 547}}; //voit1
		blured = picture.clone();
			cout << corners.size() << endl;
		for (auto&& corn: corners){
			error = _max(blur(picture, blured, corn, blur_filter_size), error);
		}
		if (error){
			DISPLAY_ERR("Couldn't blur every plate on " << filename);
			picture.release();
			blured.release();
			failed_pictures.push(filename);
			continue;
		}

		/* ==== PLATE DETECTION AND BLUR END ==== */

		// Creating directory if doesn't exist
		savedir = select_output_dir(out_dir, in_path, filepath, respect_input_path);
		fs::directory_entry dir_entry = fs::directory_entry(savedir);
		if (dir_entry.exists()){
			DISPLAY("Removing " << dir_entry.path());
			fs::remove_all(dir_entry.path());
		}
		
		if (!fs::create_directory(dir_entry.path())){
			DISPLAY_ERR("Failed to create directory " << dir_entry.path());
		}

		// Writing frames into the directory
		save_picture(blured, savedir, filename+output_name_addon+fileext);
		
		// Cleaning memory and exiting program if _timeout is reached
		if (difftime(time(NULL), t0) > _timeout){
			picture.release();
			blured.release();
			delete stack_files;
			DISPLAY("Timeout reached.");
			return EXIT_FAILURE;
		}

		picture.release();
	}

	if (verbose && !failed_pictures.empty()){ //TODO: give the possibility to save it into a file
		DISPLAY("\nSome pictures plate analysis or blur failed:")
		while (!failed_pictures.empty())
		{
			DISPLAY(failed_pictures.top());
			failed_pictures.pop();
		}
	}

	// Cleaning memory
	delete stack_files;

	DISPLAY(format("Exited successfully (%.2f seconds)", (difftime(time(NULL), t0))));

	return EXIT_SUCCESS;
}

void usage(char* name){
	cout << 
	format("\
Usage: %s -i <path to picture or directory>\
-o <output directory>\n\
Type -h or --help for more information.\n", name);
}

int main(int argc, char** argv)
{
	if (argc == 1){
		usage(argv[0]);
		return -1;
	}

	// Global variable initialization
	save_log = false;
	verbose  = false;

	// Argument declaration and defaul value
	char* in_path = new char[BUFFSIZE];
	char* out_dir = new char[BUFFSIZE];
	char *log_file = new char[BUFFSIZE];
	char *output_name_addon = new char[BUFFSIZE];
	strcpy(output_name_addon, DFLT_OUTPUT_ADDON);
	char *country = new char[BUFFSIZE];
	strcpy(country, "eu");
	double timeout = 0;
	unsigned int blur_filter_size = DFLT_BLUR;
	bool respect_input_path = false;

	/* // Parsing command line
	parse_argv(argv, in_path, out_dir,
	output_name_addon,
	timeout,
	blur_filter_size,
	verbose,
	respect_input_path,
	save_log,
	log_file,
	country); */

	/*============================ BEGIN PARSING ============================*/

	//FIXME: Stack Smashing on exit function having a "struct opttion"

	struct option options[10];

    // Retrieving argument
    options[0].long_name  = "help";
    options[0].short_name = 'h';
    options[0].flags      = GOPT_ARGUMENT_FORBIDDEN;

    options[1].long_name  = "save-log";
    options[1].short_name = 'l';
    options[1].flags      = GOPT_ARGUMENT_REQUIRED;

    options[2].long_name  = "input";
    options[2].short_name = 'i';
    options[2].flags      = GOPT_ARGUMENT_REQUIRED;

    options[3].long_name  = "output";
    options[3].short_name = 'o';
    options[3].flags      = GOPT_ARGUMENT_REQUIRED;

    options[4].long_name  = "out-name-addon";
    options[4].short_name = 'a';
    options[4].flags      = GOPT_ARGUMENT_REQUIRED;

    options[5].long_name  = "timeout";
    options[5].short_name = 't';
    options[5].flags      = GOPT_ARGUMENT_REQUIRED;

    options[6].long_name  = "blur-power";
    options[6].short_name = 'p';
    options[6].flags      = GOPT_ARGUMENT_REQUIRED;

    options[7].long_name  = "verbose";
    options[7].short_name = 'v';
    options[7].flags      = GOPT_ARGUMENT_FORBIDDEN;

    options[8].long_name  = "respect-path";
    options[8].short_name = 'r';
    options[8].flags      = GOPT_ARGUMENT_FORBIDDEN;

	options[9].long_name  = "country";
    options[9].short_name = 'c';
    options[9].flags      = GOPT_ARGUMENT_REQUIRED;

    options[10].flags      = GOPT_LAST;

    gopt (argv, options);
    gopt_errors (argv[0], options);

    // Setting variables if necessary
    if (options[0].count){
        cout << "\
Usage: blur -i <path to video or directory> -o <output directory> [args]\n\
Requiered argument:\n\
\t- -i or --input:  The path to the input file or directory.\n\
\t- -o or --output: The path to the output directory. Will be created if doesn't exist.\n\
Optional argument:\n\
\t- -h or --help:           Displays this screen.\n\
\t- -l or --save-log:       The path to a file where all the logs will be saved.Will be created if doesn't exist.\n\
\t- -a or --out-name-addon: The name addon for every blured picture (default: '_rendered').\n\
\t- -t or --timeout:        A timeout in seconds.\n\
\t- -p or --blur-power:     The size of the square box used to make a blur effect (default: 70).\n\
\t- -v or --verbose:        Whether or not informations has to be displayed. This does not affect the logs.\n\
\t- -r or --respect-path:   Whether or not the path of output blured picture has to be similar to their path in the input directory.\n\
";
        exit(EXIT_SUCCESS);
    }
	if (!options[2].count || !options[3].count){
        cerr << "Input path and output directory needs to be specified.\n";
        exit(EXIT_FAILURE);
    }
    if (options[1].count){
        save_log = true;
        strcpy(log_file, options[1].argument); 
    }
    if (options[2].count) strcpy(in_path, options[2].argument);
    if (options[3].count) strcpy(out_dir, options[3].argument);
    if (options[4].count) strcpy(output_name_addon, options[4].argument);
    if (options[5].count) timeout = atof(options[5].argument);
    if (options[6].count) blur_filter_size = atoi(options[6].argument);
    if (options[7].count) verbose = true;
    if (options[8].count) respect_input_path = true;
	if (options[9].count) strcpy(country, options[9].argument);;

	/*============================ END PARSING ============================*/

	// Executing main process
	process(in_path, out_dir,
			output_name_addon,
			timeout,
			blur_filter_size,
			respect_input_path,
			log_file,
			country);

	delete[] in_path;
	delete[] out_dir;
	delete[] output_name_addon;
	delete[] log_file;
	delete[] country;

    return EXIT_SUCCESS;
}

