#include "options.h"
#include "gopt/gopt.h"
#include "utils.h"
#include "config.h"

#include <cstdlib>
#include <iostream>
#include <string.h>
#include <cstring>
#include <cstdio>
#include <filesystem>

using namespace std;
namespace fs = filesystem;

#define HELP_PROMPT "\
Usage: -i <path to picture or directory> -o <output directory> [args]\n\
Requiered argument:\n\
\t- -i or --input:  The path to the input file or directory.\n\
\t- -o or --output: The path to the output file or directory. \n\
\t                  Output file path is only available with a file as input.\n\
\t                  Directories will be created if they don't exist.\n\
Optional argument:\n\
\t- -h or --help:           Displays this screen.\n\
\t- -l or --save-log:       The path to a file where all the logs will be saved.Will be created if doesn't exist.\n\
\t- -a or --out-name-addon: The name addon for every blurred picture (default: '_rendered').\n\
\t- -t or --timeout:        A timeout in seconds.\n\
\t- -b or --blur-power:     The size of the square box used to make a blur effect (default: 70).\n\
\t- -v or --verbose:        Whether or not informations has to be displayed. This does not affect the logs.\n\
\t- -c or --counry_code:    The country code of the car, to match the country's plate pattern. (\"eu\", \"us\", \"au\", \"br\", \"fr\", \"gb\", \"in\". Default: \"eu\")\n\
\t- -s or --save-info:      Whether or not plate information sould be saved as well.\n\
\t- -p or --respect-path:   Whether or not the path of output blurred picture has to be similar to their path in the input directory.\n\
\t- -r or --rename:         Rename input images with _origin and replace it with the blurred image.\n\
"
//\t- -s or --save-info:      Whether or not plate information sould be saved as well. If the input is a file, and output path for this can be given.\n

void parse_argv(char **argv, char* in_path, char *out_dir,
	char *output_name_addon,
	double &timeout,
	unsigned int &blur_filter_size,
	bool &verbose,
	bool &respect_input_path,
    bool &save_log,
	char *log_file,
    char *country,
    bool &save_plate_info,
    char *plate_info_save_path,
    bool &blur_only,
    char *blur_only_location,
    bool &replace_input_file){
    
	struct option *options = new option[14];

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
    options[6].short_name = 'b';
    options[6].flags      = GOPT_ARGUMENT_REQUIRED;

    options[7].long_name  = "verbose";
    options[7].short_name = 'v';
    options[7].flags      = GOPT_ARGUMENT_FORBIDDEN;

    options[8].long_name  = "respect-path";
    options[8].short_name = 'p';
    options[8].flags      = GOPT_ARGUMENT_FORBIDDEN;

	options[9].long_name  = "counry_code";
    options[9].short_name = 'c';
    options[9].flags      = GOPT_ARGUMENT_REQUIRED;

	options[10].long_name  = "save-info";
    options[10].short_name = 's';
    options[10].flags      = GOPT_ARGUMENT_OPTIONAL;

	options[11].long_name  = "blur-only";
    options[11].short_name = 'w';
    options[11].flags      = GOPT_ARGUMENT_REQUIRED;

    options[12].long_name  = "rename";
    options[12].short_name = 'r';
    options[12].flags      = GOPT_ARGUMENT_OPTIONAL;

    options[13].flags      = GOPT_LAST;

    gopt(argv, options);
    gopt_errors(argv[0], options);

    // Setting variables if necessary
    if (options[0].count){
        cout << HELP_PROMPT;
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
	if (options[9].count) strcpy(country, options[9].argument);
    if (options[10].count){
        save_plate_info = true;
        if (options[10].argument != NULL){ //FIXME: options[10].argument == NULL even if argument is provided
            plate_info_save_path = new char[strlen(options[10].argument) + 1];
            strcpy(plate_info_save_path, options[10].argument); 
        }
    }
    if (options[11].count) {
        blur_only = true;
        strcpy(blur_only_location, options[11].argument);
    }
    if (options[12].count) replace_input_file = true;

    // Options compatibility verification
    if (blur_only && save_plate_info){
        DISPLAY_WAR("--blur-only is not compatible with --save-info."
        << "\nDisabling --save-info.\n")
        save_plate_info = false;
        delete[] plate_info_save_path;
        plate_info_save_path = NULL;
    }

    if (fs::directory_entry(in_path).is_regular_file() && plate_info_save_path != NULL){
        DISPLAY_WAR("--save-info path location is not compatible with a file as input. Please use it for directories instead."
        << "\nDisabling --save-info path. (--save-info remains enabled)\n")
        delete[] plate_info_save_path;
        plate_info_save_path = NULL;
    }

    if (fs::directory_entry(in_path).is_regular_file() && respect_input_path){
        DISPLAY_WAR("--respect-path is not compatible with a file as input. Please use it for directories instead."
        << "\nDisabling --respect-path.\n")
        respect_input_path = false;
    }

    if (blur_only && options[9].count){ // country
        DISPLAY_WAR("--blur-only does not require --counry_code."
        << "\nIgnoring country code.\n")
    }

    if (!options[4].count && replace_input_file){
        strcpy(output_name_addon, DFLT_BACKUP_ADDON);
    }

    delete[] options;
}
