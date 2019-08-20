#include "utils.h"
#include "blur.h"
#include "options.h"
#include "gopt/gopt.h"

#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/core/mat.hpp>
#include <alpr.h>

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
#ifndef DFLT_CONFIG_FILE
#define DFLT_CONFIG_FILE "/usr/local/share/openalpr/config/openalpr.defaults.con"
#endif
#ifndef DFLT_RUNTIME_DIR
#define DFLT_RUNTIME_DIR "/usr/local/share/openalpr/runtime_data/"
#endif
#define DFLT_FAILED_PIC_DIR "blur_failure_files.txt"
#define BUFFSIZE 200

bool verbose; /// Whether or not information should be displayed
bool save_log; /// Whether or not logs should be saved (default: false)
ofstream log_ostream; /// Stream to the file where the logs will be saved

/**
 * @brief Retireves the plates corners and number from results to corners and numbers
 * 
 * @param results the returned value of the alpr process
 * @param corners a buffer where to write corners (from top-left following the chonological order)
 * @param numbers a buffer where to write the detected plates
 */
void plate_corners(const vector<AlprPlateResult> &results,
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
		tmp_vect.clear();
	}
}

// * @param respect_input_path whether or not the initial input path should be resected
/**
 * @brief Main process
 * 
 * @param in_path the path to the inpu directory or picture file
 * @param out_path the path to the output file if input is a file, output directory else
 * @param output_name_addon the characters added before the extensions on rendered images
 * @param timeout a timeout in seconds
 * @param blur_filter_size the size of the square area used to make the blur effect
 * @param log_file the path to the file containing all the logs (displayed text) for a given runtime
 * @param country the country code for the plate detection
 * @param save_plate_info whether or not alpr's plate recognition have to be saved (in json format).
 * @return int 0 for success, 1 else
 */
int process(const char* in_path, const char* out_path,
			const char *output_name_addon,
			const double timeout,
			const unsigned int blur_filter_size,
			const bool respect_input_path,
			const char *log_file,
			const char *country,
			const bool save_plate_info,
			const char *plate_info_save_path,
			const bool blur_only,
			const char *blur_only_location){

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
	const size_t nb_pictures = stack_files->size(); //For success rate
	//const vector country_code_vect = {"eu", "fr", "gb", "us", "au", "br", "in"};

	// Blur only input verification
	if (blur_only){
		if (stack_files->size() != 1 || !fs::directory_entry(in_path).is_regular_file()){
			cerr << "\033[1;31mERROR:\033[0m Only 1 file can be given for blur only.\n";
			exit(EXIT_FAILURE);
		}
	}

	// Main loop
	while (!stack_files->empty()){
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

		/* ==== PLATE DETECTION AND BLUR BEGIN ==== */

		// Buffers to get ALPR results
		vector<vector<Point> > corners = vector<vector<Point> >(); /// Detected plates : [plate1: [tl, tr, br, bl], plate2: ...]
		vector<string> numbers = vector<string>();;
		Alpr detector = Alpr(country, DFLT_CONFIG_FILE, DFLT_RUNTIME_DIR);
		AlprResults alpr_results;

		if (!blur_only){
			// Getting picture's plate informations
			alpr_results = detector.recognize(filepath);
			vector<AlprPlateResult> results = alpr_results.plates;
			plate_corners(results, corners, numbers);
			if (corners.size() == 0){
				DISPLAY_ERR("No plate detected on " << filename);
				failed_pictures.push(filepath);
				continue;
			}
			
			// Displaying the plates numbers
			if (verbose || save_log){
				DISPLAY("Plates detected:")
				for (auto&& num: numbers){
					DISPLAY(num);
				}
			}
		} else {
			// Parsing corners and verifying validity
			corners = parse_location(blur_only_location);
			for (auto&& cor: corners){
				for (auto&& pt: cor){
					if (pt.x < 0 || pt.y < 0 || pt.x > picture.cols || pt.y > picture.rows){
						cerr << "\033[1;31mERROR:\033[0m Invalid pixel location: " << blur_only_location
						<< "\nPlease respect format x11_y11_x12_y12_x13_y13_x14_y14_x21_y21.. with valid values." << endl;
						exit(EXIT_FAILURE);
					}
				}
				sort_corners(cor); //NOT IMPLEMENTED
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
			//if ((unsigned int) error == corners.size()){ //Skip only if every blur failed
				failed_pictures.push(filepath);
				continue;
			//}
		}

		/* ==== PLATE DETECTION AND BLUR END ==== */


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

		// Saving the plate information if necessary
		if (!blur_only && save_plate_info && !numbers.empty()){
			string plate_file = savedir+"/"+filename+DFLT_JSON_ADDON+".json";
			// If -s option has a path given, behavior is the same as above
			if (fs::directory_entry(in_path).is_regular_file() && plate_info_save_path != NULL){
				if (build_directories(plate_info_save_path)){
					if (fs::path(plate_info_save_path).has_extension()){
						plate_file = plate_info_save_path;
					} else {
						plate_file = ((string) plate_info_save_path)+"/"+filename+DFLT_JSON_ADDON+".json";
					}
				}
			}
			ofstream plate_ostream;
			plate_ostream.open(plate_file);
			if (!plate_ostream){
				DISPLAY_ERR("Couldn't open " << plate_file);
			} else {
				plate_ostream << detector.toJson(alpr_results);
				DISPLAY("Wrote plate info " << plate_file)
			}
			plate_ostream.close();
		}
		
		// Cleaning memory and exiting program if _timeout is reached
		if (difftime(time(NULL), t0) > _timeout){			
			delete stack_files;
			DISPLAY("Timeout reached.");
			return EXIT_SUCCESS;
		}
		
	}

	const size_t nb_failed = failed_pictures.size(); // For success rate
	if ((verbose || save_log) && !failed_pictures.empty()){
		DISPLAY("\nSome pictures plate analysis or blur failed:")

		// Opening file containing failed pictures only
		ofstream failedpic_stream;
		failedpic_stream.open(DFLT_FAILED_PIC_DIR);
		if (!failedpic_stream){
			DISPLAY_ERR("Couldn't open " << DFLT_FAILED_PIC_DIR)
		}

		// Displaying and writing failed pictures path
		while (!failed_pictures.empty())
		{
			DISPLAY(failed_pictures.top());
			failedpic_stream << failed_pictures.top() << endl;
			failed_pictures.pop();
		}
	}

	DISPLAY("\nExited successfully (" << difftime(time(NULL), t0) << " seconds). "
	<< "Success rate: " << 100*((double) (nb_pictures - nb_failed)/(double) nb_pictures) << " %");

	// Cleaning memory
	delete stack_files;

	return EXIT_SUCCESS;
}

void usage(char* name){
	cout <<"\
Usage: " << name << " -i <path to picture or directory>\
-o <output directory>\n\
Type -h or --help for more details.\n";
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
	char* out_path = new char[BUFFSIZE];
	char *output_name_addon = new char[BUFFSIZE];
	strcpy(output_name_addon, DFLT_OUTPUT_ADDON);
	double timeout = 0;
	unsigned int blur_filter_size = DFLT_BLUR;
	bool respect_input_path = false;
	char *log_file = new char[BUFFSIZE];
	char *country = new char[BUFFSIZE];
	strcpy(country, DFLT_COUNTRY);
	bool save_plate_info = false;
	char *plate_info_save_path = NULL; // Dynamically allocated if argument provided
	bool blur_only = false;
	char *blur_only_location = new char[BUFFSIZE];

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
			   blur_only_location);

	// Executing main process
	int ret = process(in_path, out_path,
					  output_name_addon,
					  timeout,
					  blur_filter_size,
					  respect_input_path,
					  log_file,
					  country,
					  save_plate_info,
					  plate_info_save_path,
					  blur_only,
					  blur_only_location);

	delete[] in_path;
	delete[] out_path;
	delete[] log_file;
	delete[] output_name_addon;
	delete[] country;
	delete[] blur_only_location;
	if (plate_info_save_path != NULL) delete[] plate_info_save_path;

    return ret;
}

