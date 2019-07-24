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
#define DFLT_JSON_ADDON "_info"
#define DFLT_BLUR 70
#define DFLT_COUNTRY "eu"
#define DFLT_CONFIG_FILE "/home/broseau/ProgrammePapa/ImPlate/alpr/config/openalpr.conf.defaults"
#define DFLT_RUNTIME_DIR "/home/broseau/ProgrammePapa/ImPlate/alpr/runtime_data"
#define BUFFSIZE 200

bool verbose;
bool save_log;
ofstream log_ostream; // To make log-file available everywhere

/* TODO:
	- Fix "respect original path"
	- Better blur (or use OpenCV's blur)
	- Cmake
	- Ajouter dans la doc les options dispo pour le pays de la plaque
	- Formater le json pour que ce soit plus simple à lire..

	Pour OpenCV et OpenALPR: les compiler à part, et linker directement les .so
 */

/**
 * @brief Retireves the plates corners and number from results to corners and numbers
 * 
 * @param results the returned value of the alpr process
 * @param corners a buffer where to write corners (from top-left following the chonological order)
 * @param numbers a buffer where to write the detected plates
 */
void plateCorners(const vector<AlprPlateResult> &results,
				 vector<vector<Point> > &corners,
				 vector<string> &numbers){
	size_t i, j;
	vector<Point> tmp_vect;
	Point tmp_pt;
	for (i = 0; i < results.size(); i++){
		for (j = 0; j < 4; j++){
			tmp_pt.x = results[i].plate_points[j].x;
			tmp_pt.y = results[i].plate_points[j].y;
			tmp_vect.push_back(tmp_pt);
		}
		corners.push_back(tmp_vect); 
		numbers.push_back(results[i].bestPlate.characters);
	}
}

int process(const char* in_path, const char* out_dir,
			const char *output_name_addon,
			const double timeout,
			const unsigned int blur_filter_size,
			const bool respect_input_path,
			const char *log_file,
			const char *country,
			const bool save_plate_info){

	// Building output directory if it doesn't exists
	if (build_dir(out_dir)){
		cerr << "ERROR: Couldn't build output directory.\n";
		exit(EXIT_FAILURE);
	}

	// Opening log file if necessary
	if (save_log){
		log_ostream.open(log_file);
		if (!log_ostream){
			cerr << format("ERROR: Couldn't open %s\n", log_file);
			exit(EXIT_FAILURE);
		}
	}

	// Retrieving picture file paths into stack_files
	stack<string> *stack_files = list_files(in_path);
	if (stack_files == NULL){
		cerr << "ERROR: Couldn't open any file.\n";
		exit(EXIT_FAILURE);
	}

	// Parameters initialisation
	stack<string> failed_pictures = stack<string>();
	Mat picture = Mat();
	Mat blured = Mat();
	string filepath, filename, fileext, savedir, file_out_dir;
	int error;
	unsigned int loop_idx = 0;
	unsigned int nb_files = stack_files->size();
	double _timeout = timeout == 0 ? DBL_MAX : timeout; 
	double t0 = time(NULL);

	// Main loop
	while (!stack_files->empty()){
		// Getting file path informations
		filepath = stack_next(stack_files);
		filename = get_filename(filepath);
		fileext  = get_file_extension(filepath);
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

		// Buffers to get ALPR results
		vector<vector<Point> > corners = vector<vector<Point> >();
		vector<string> numbers = vector<string>();
		
		// Getting picture's plate informations
		Alpr detector = Alpr(country, DFLT_CONFIG_FILE, DFLT_RUNTIME_DIR);
		AlprResults alpr_results = detector.recognize(filepath);
		vector<AlprPlateResult> results = alpr_results.plates;
		plateCorners(results, corners, numbers);
		if (corners.size() == 0){
			DISPLAY_ERR("No plate detected on " << filename
			<< "\nAre you sure the country code for this car is \"" << country << "\" ?");
			failed_pictures.push(filename);
			continue;
		}

		// Displaying the plates numbers
		if (verbose || save_log){
			DISPLAY("Plates detected:")
			for (auto&& num: numbers){
				DISPLAY(num);
			}
		}

		// Bluring a copy of the initial picture
		blured = picture.clone();
		error = 0;
		for (auto&& corn: corners){
			error += _max(blur(picture, blured, corn, blur_filter_size), error);
		}
		if (error){
			DISPLAY_ERR("Couldn't blur" << error << " times out of " << corners.size() << " on " << filename);
			if ((unsigned int) error == corners.size()){
				failed_pictures.push(filename);
				continue;
			}
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

		// Saving the plate information if necessary
		if (save_plate_info && !numbers.empty()){
			string plate_file = savedir+"/"+filename+DFLT_JSON_ADDON+".json";
			ofstream plate_ostream;
			plate_ostream.open(plate_file);
			if (!plate_ostream){
				DISPLAY_ERR("Couldn't open" << plate_file);
			} else {
				plate_ostream << detector.toJson(alpr_results);
			}
			plate_ostream.close();
		}

		// Writing blured picture into the directory
		save_picture(blured, savedir, filename+output_name_addon+fileext);
		
		// Cleaning memory and exiting program if _timeout is reached
		if (difftime(time(NULL), t0) > _timeout){			
			delete stack_files;
			DISPLAY("Timeout reached.");
			return EXIT_SUCCESS;
		}
		
	}

	if ((verbose || save_log) && !failed_pictures.empty()){
		DISPLAY("\nSome pictures plate analysis or blur failed:")
		while (!failed_pictures.empty())
		{
			DISPLAY(failed_pictures.top());
			failed_pictures.pop();
		}
	}

	// Cleaning memory
	delete stack_files;

	DISPLAY(format("\nExited successfully (%.2f seconds)", (difftime(time(NULL), t0))));

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
	strcpy(country, DFLT_COUNTRY);
	double timeout = 0;
	unsigned int blur_filter_size = DFLT_BLUR;
	bool respect_input_path = false;
	bool save_plate_info = false;

	// Parsing command line
	parse_argv(argv, in_path, out_dir,
			   output_name_addon,
			   timeout,
			   blur_filter_size,
			   verbose,
			   respect_input_path,
			   save_log,
			   log_file,
			   country,
			   save_plate_info);

	// Executing main process
	int ret = process(in_path, out_dir,
					  output_name_addon,
					  timeout,
					  blur_filter_size,
					  respect_input_path,
					  log_file,
					  country,
					  save_plate_info);

	delete[] in_path;
	delete[] out_dir;
	delete[] log_file;
	delete[] output_name_addon;
	delete[] country;

    return ret;
}

