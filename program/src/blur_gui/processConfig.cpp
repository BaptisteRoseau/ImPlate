#include "utils.h"
#include "blur.h"
#include "options.h"
#include "gopt/gopt.h"
#include "config.h"
#include "processConfig.h"

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
#include <set>
#include <limits.h>

using namespace std;
using namespace cv;
using namespace alpr;

namespace fs = filesystem;

#define ARGS_BUFFSIZE 200

extern bool verbose; /// Whether or not information should be displayed
extern bool save_log; /// Whether or not logs should be saved (default: false)
extern ofstream log_ostream; /// Stream to the file where the logs will be saved


/* =====================================================
             INITIALIZATION / DESTRUCTION
   =====================================================*/

ProcessConfig::ProcessConfig(char *argv[]){

	// Argument declaration and default value
	this->in_path           = new char[ARGS_BUFFSIZE];
	this->out_path          = new char[ARGS_BUFFSIZE];
	this->output_name_addon = new char[ARGS_BUFFSIZE];
	strcpy(this->output_name_addon, DFLT_OUTPUT_ADDON);
	this->blur_filter_size = DFLT_BLUR;
	this->respect_input_path = false;
	this->log_file = new char[ARGS_BUFFSIZE];
	this->country  = new char[ARGS_BUFFSIZE];
	strcpy(this->country, DFLT_COUNTRY);
	this->save_plate_info = false;
    this->plate_info_save_path = NULL; // Dynamically allocated if argument provided
	this->replace_input_file = false;

    // Useless but required to use parse_argv() 
	double timeout = 0;
	bool blur_only = false;
	char *blur_only_location = new char[ARGS_BUFFSIZE];

	// Parsing command line
	parse_argv(argv, this->in_path, this->out_path,
			   this->output_name_addon,
			   timeout,
			   this->blur_filter_size,
			   verbose,
			   this->respect_input_path,
			   save_log,
			   this->log_file,
			   this->country,
			   this->save_plate_info,
			   this->plate_info_save_path,
			   blur_only,
			   blur_only_location,
			   this->replace_input_file);

    delete[] blur_only_location;

    // Initializing other parameters
	filepath = filename = fileext = savedir = loop_idx = nb_files = 0;
    this->stack_files      = NULL;
    this->stack_files_done = new stack<string>();
	this->success_pictures = new set<string>();
    this->failed_pictures  = new set<string>();
    this->detector = new Alpr(this->country, DFLT_CONFIG_FILE, DFLT_RUNTIME_DIR);
    this->alpr_results = AlprResults();
}


ProcessConfig::~ProcessConfig(){
    delete[] in_path;
	delete[] out_path;
	delete[] output_name_addon;
	delete[] log_file;
	delete[] country;
    if (plate_info_save_path != NULL) delete[] plate_info_save_path;
    delete stack_files;
    delete stack_files_done;
    delete success_pictures;
    delete failed_pictures;
}


/* =====================================================
                    INIT AND FINALIZE
   =====================================================*/

void ProcessConfig::init(void){
	// Retrieving picture file paths into stack_files
	this->stack_files = list_files(in_path);
	if (stack_files == NULL){
		//Don't use DISPLAY_ERR as you want to see this message even without verbose
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
	this->picture = Mat();
	this->blurred  = Mat();
    this->loop_idx = 0;
	this->nb_files = stack_files->size();
	//const vector country_code_vect = {"eu", "fr", "gb", "us", "au", "br", "in"};
}

void ProcessConfig::finalize(void){
    // Cleaning failed pictures
    for (auto &succ_pic: *success_pictures){
        this->failed_pictures->erase(succ_pic);
    }

	// Displaying success pictures
	if ((verbose || save_log) && !this->success_pictures->empty()){
		DISPLAY("\nPictures successfully blurred:")

		// Opening file containing success pictures only
		ofstream successpic_stream;
		successpic_stream.open(DFLT_SUCCESS_PIC_FILE);
		if (!successpic_stream){
			DISPLAY_ERR("Couldn't open " << DFLT_SUCCESS_PIC_FILE)
		}

		// Displaying and writing success pictures path
        for (auto &pic_path: *success_pictures){
            DISPLAY(pic_path);
			successpic_stream << pic_path << endl;
        }
	}

	// Displaying failed pictures
	if ((verbose || save_log) && !this->failed_pictures->empty()){
		DISPLAY("\nSome pictures plate analysis or blur failed:")

		// Opening file containing failed pictures only
		ofstream failedpic_stream;
		failedpic_stream.open(DFLT_FAILURE_PIC_FILE);
		if (!failedpic_stream){
			DISPLAY_ERR("Couldn't open " << DFLT_FAILURE_PIC_FILE)
		}

        // Displaying and writing success pictures path
        for (auto &pic_path: *failed_pictures){
            DISPLAY(pic_path);
			failedpic_stream << pic_path << endl;
        }
	}
}


/* =====================================================
                    METHODS
   =====================================================*/

bool ProcessConfig::isFirstImage(void){
    return this->stack_files_done->empty();
}

bool ProcessConfig::isLastImage(void){
    return this->stack_files->empty();
}

int ProcessConfig::updatePathAndPicture(void){
    this->filename = get_filename(this->filepath);
    this->fileext  = get_file_extension(this->filepath);

    // Opening picture
    this->picture = open_picture(this->filepath);
    if (picture.empty()){
        DISPLAY_ERR("Couldn't open " << this->filepath);
        this->failed_pictures->insert(this->filepath);
        return EXIT_FAILURE;
    }
    this->blurred = this->picture.clone();
    this->clearCornersVector();
    this->clearAutoblurPlateInfo();

    return EXIT_SUCCESS;
}

Mat ProcessConfig::getblurredPicture(void){
    return this->blurred;
}

string ProcessConfig::getFilepath(void){
    return this->filepath;
}

bool ProcessConfig::isPictureStateChanged(void){
    Mat diff;
    absdiff(this->picture, this->blurred, diff);
    cvtColor(diff, diff, COLOR_BGR2GRAY);
    return  countNonZero(diff) != 0;
}

int ProcessConfig::currentPictureIdx(void){
    return this->stack_files_done->size() + 1;
}

int ProcessConfig::maximumPictureIdx(void){
    return this->stack_files->size() + this->stack_files_done->size();
}

void ProcessConfig::setCornersVector(vector<Point> v){
    this->corners = v;
}

void ProcessConfig::clearCornersVector(void){
    this->corners.clear();
}

void ProcessConfig::clearAutoblurPlateInfo(void){
    this->alpr_results = AlprResults();
}

/* =====================================================
                    BUTTON METHODS
   =====================================================*/

int ProcessConfig::firstImage(void){
    // Getting file path informations
    this->filepath = stack_next(this->stack_files);
    this->loop_idx++;

    // Displaying script advancement
    DISPLAY("\nPicture: " << loop_idx << " out of " << nb_files
            << " (" << (float) 100*loop_idx/nb_files << " %) : " << filename)

    return this->updatePathAndPicture();
}

int ProcessConfig::previousImage(void){
    if (this->isFirstImage()){
        DISPLAY_WAR("This is the first image.");
        return EXIT_FAILURE;
    }

    // Getting file path informations
    this->stack_files->push(this->filepath);
    this->filepath = stack_next(this->stack_files_done);
    this->loop_idx--;

    // Displaying script advancement
    DISPLAY("\nPicture: " << loop_idx << " out of " << nb_files
            << " (" << (float) 100*loop_idx/nb_files << " %) : " << filename)

    return this->updatePathAndPicture();
}

int ProcessConfig::nextImage(void){
    if (this->isLastImage()){
        DISPLAY_WAR("This is the last image.");
        return EXIT_FAILURE;
    }

    // Getting file path informations
    this->stack_files_done->push(this->filepath);
    this->filepath = stack_next(this->stack_files);
    this->loop_idx++;

    // Displaying script advancement
    DISPLAY("\nPicture: " << loop_idx << " out of " << nb_files
            << " (" << (float) 100*loop_idx/nb_files << " %) : " << filename)

    return this->updatePathAndPicture();
}

int ProcessConfig::blurImage(vector<vector<Point> > corners){
    for (auto&& corn: corners){
        if (corn.size() != 4){
            DISPLAY_ERR("Invalid amount of corners:" << corn.size() << "(Need 4).");
            return EXIT_FAILURE;
        }    
    }

    // Bluring this->blurred based on this->picture pixels
    int error = 0;
    for (auto&& corn: corners){
        error += blur(this->picture, this->blurred, corn, this->blur_filter_size);
    }
    if (error){
        DISPLAY_ERR("Couldn't blur " << error << " times out of " << corners.size() << " on " << filename);
        //if ((unsigned int) error == corners.size()){ //Skip only if every blur failed
            failed_pictures->insert(filepath);
            return EXIT_FAILURE;
        //}
    }

    return EXIT_SUCCESS;
}

int ProcessConfig::saveImage(void){
    // If input is a file, output will be directly out_path, not a directory
    if (fs::directory_entry(in_path).is_regular_file()){
        if (build_directories(out_path)){
            if (fs::path(out_path).has_extension()){
                if (replace_input_file){
                    // Replacing input file if "--rename" option and it doesn't exist yet
                    if (!fs::exists(fs::path(out_path))) { fs::rename(filepath, out_path); }
                    savedir = (string) fs::path(out_path).parent_path(); //Used for save-info
                    save_picture(blurred, filepath);
                } else {
                    // Saving file as in given
                    savedir = (string) fs::path(out_path).parent_path();
                    save_picture(blurred, out_path);
                }
            } else {
                if (replace_input_file){
                    // Replacing input file if "--rename" option and it doesn't exist yet
                    string tmp = (string)out_path+"/"+filename+output_name_addon+fileext;
                    if (!fs::exists(fs::path(tmp))) { rename(filepath, tmp); }
                    savedir = (string) out_path; //Used for save-info
                    save_picture(blurred, filepath);
                } else {
                    // Saving file into the directory given
                    savedir = out_path;
                    save_picture(blurred, out_path, filename+output_name_addon+fileext);
                }
            }
        } else {
            DISPLAY_ERR("An error occurred during saving.");
            return EXIT_FAILURE;
        }
    // If input is a directory
    } else {
        // Selecting output directory and building required directories
        savedir = remove_dots(select_output_dir(out_path, in_path, filepath, respect_input_path));
        if (!build_directories(savedir)){
            DISPLAY_ERR("Couldn't build directories for " << savedir);
            failed_pictures->insert(filepath);
            return EXIT_FAILURE;
        }
        if (replace_input_file){
            // Replacing input file
            string tmp = savedir+"/"+filename+output_name_addon+fileext;
            if (!fs::exists(fs::path(tmp))) { rename(filepath, tmp); }
            save_picture(blurred, filepath);
        } else {
            // Writing blurred picture into the directory
            save_picture(blurred, savedir, filename+output_name_addon+fileext);
        }
    }

    // Saving the plate information if necessary
    if (save_plate_info){
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
            if (!this->alpr_results.plates.empty()){ // Saving info from ALPR
                plate_ostream << this->detector->toJson(this->alpr_results);
                DISPLAY("Wrote plate info " << plate_file)
            } else if (!this->corners.empty()) { // Saving info from picture info and clicked corners
                // Yeah, I know this is disgusting, but it was so much faster than a dictionary and an API :/
                plate_ostream << "{\"version\":2,\"data_type\":\"blur_gui\",\"img_width\":"
                << this->blurred.cols << ",\"img_height\":" << this->blurred.rows << 
                ",\"regions_of_interest\":[{\"x\":0,\"y\":0,\"width\":" << this->blurred.cols
                << ",\"height\":" << this->blurred.rows << "}],\"results\":[{\"coordinates\":[{\"x\":"
                << this->corners[0].x << ",\"y\":" << this->corners[0].y << "},{\"x\":"
                << this->corners[1].x << ",\"y\":" << this->corners[1].y << "},{\"x\":"
                << this->corners[2].x << ",\"y\":" << this->corners[2].y << "},{\"x\":"
                << this->corners[3].x << ",\"y\":" << this->corners[3].y << "}]}]}";
                DISPLAY("Wrote plate info " << plate_file)
            } else {
                DISPLAY_WAR("No plate coordinate info to be saved: "
                << "Removing previous plate info results.");
                remove(plate_file);
            }
        }
        plate_ostream.close();
    }

    // Picture was successfully saved
    success_pictures->insert(filepath);
    
    return this->updatePathAndPicture();;
}

int ProcessConfig::cancel(void){
    // If blurred picture has already been saved on disk, then remove it
    if (!this->isPictureStateChanged()){
        // If input is a file, output will be directly out_path, not a directory
        if (fs::directory_entry(in_path).is_regular_file()){
                if (fs::path(out_path).has_extension()){
                    savedir = (string) fs::path(out_path).parent_path();
                    if (fs::exists(fs::path(savedir))){
                        if (replace_input_file){
                            // Getting input file back to its original path
                            rename(out_path, filepath);
                        } else {
                            // Removing blurred picture from the output directory
                            remove(out_path);
                        }
                    }
                } else {
                    savedir = out_path;
                    if (fs::exists(fs::path(savedir))){
                        if (replace_input_file){
                            // Getting input file back to its original path
                            rename((string)out_path+"/"+filename+output_name_addon+fileext, filepath);
                        } else {
                            // Removing blurred picture from the output directory
                            remove((string)out_path+"/"+filename+output_name_addon+fileext);
                        }
                    }
                }
        // If input is a directory, 
        } else {
            // Selecting output directory
            savedir = select_output_dir(out_path, in_path, filepath, respect_input_path);
            if (fs::exists(fs::path(savedir))){
                if (replace_input_file){
                    // Getting input file back to its original path
                    rename(savedir+"/"+filename+output_name_addon+fileext, filepath);
                } else {
                    // Removing blurred picture from the output directory
                    remove(savedir+'/'+filename+output_name_addon+fileext);
                }
            }
        }

        // Removing plate info if exist
        if (save_plate_info && fs::exists(fs::path(savedir))){
            string plate_file = savedir+"/"+filename+DFLT_JSON_ADDON+".json";
            // If -s option has a path given, behavior is the same as above
            if (fs::directory_entry(in_path).is_regular_file() && plate_info_save_path != NULL){
                if (fs::path(plate_info_save_path).has_extension()){
                    plate_file = plate_info_save_path;
                } else {
                    plate_file = ((string) plate_info_save_path)+"/"+filename+DFLT_JSON_ADDON+".json";
                }
            }
            if (fs::exists(fs::path(plate_file))){
                if (fs::remove(fs::path(plate_file))){
                    DISPLAY("Removed " << plate_file);
                } else {
                    DISPLAY_ERR("Unable to remove " << plate_file);
                }
            }
        }

        // Removing all empty directories above the saving one
        if (fs::exists(fs::path(savedir))) remove_empty_directories(savedir);
    }

    // Updating pictures
    this->success_pictures->erase(filepath);

    return this->updatePathAndPicture();;
}

int ProcessConfig::autoBlur(void){
    // Buffers to get ALPR results
    vector<vector<Point> > corners = vector<vector<Point> >(); /// Detected plates : [plate1: [tl, tr, br, bl], plate2: ...]
    vector<string> plate_numbers = vector<string>();

    // Getting picture's plate informations
    this->alpr_results = this->detector->recognize(this->filepath);
    vector<AlprPlateResult> results = alpr_results.plates;
    plate_corners(results, corners, plate_numbers);
    if (corners.size() == 0){
        DISPLAY_WAR("No plate detected on " << filename);
        failed_pictures->insert(filepath);
        return EXIT_FAILURE;
    }
    
    // Displaying the plates numbers
    if (verbose || save_log){
        DISPLAY("Plates detected:")
        for (auto&& num: plate_numbers){
            DISPLAY(num);
        }
    }

    return this->blurImage(corners);
}
