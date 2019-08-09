#include "options.h"
#include "gopt/gopt.h"

#include <cstdlib>
#include <iostream>
#include <string.h>
#include <cstring>
#include <cstdio>

using namespace std;

#define HELP_PROMPT "\
Usage: -i <path to picture or directory> -o <output directory> [args]\n\
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
\t- -c or --counry:         The country code of the car, to match the country's plate pattern. (default: \"eu\")\n\
\t- -s or --save-info:      Whether or not plate information sould be saved as well.\n\
"
// \t- -r or --respect-path:   Whether or not the path of output blured picture has to be similar to their path in the input directory.(Not working yet)\n\


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
    bool &blur_only,
    char *blur_only_location){
    
	struct option *options = new option[13];

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

	options[10].long_name  = "save-info";
    options[10].short_name = 's';
    options[10].flags      = GOPT_ARGUMENT_FORBIDDEN;

	options[11].long_name  = "blur-only";
    options[11].short_name = 'b';
    options[11].flags      = GOPT_ARGUMENT_REQUIRED;

    options[12].flags      = GOPT_LAST;

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
    if (options[10].count) save_plate_info = true;
    if (options[11].count) {
        blur_only = true;
        strcpy(blur_only_location, options[11].argument);
    }

    delete[] options;
}
