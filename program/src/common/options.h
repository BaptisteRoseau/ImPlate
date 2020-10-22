#ifndef OPTIONS_HPP
#define OPTIONS_HPP

/**
 * @brief Parse the command line to set the variable required by the main process.
 * The variable's default value must have been set before the function is called.
 * 
 * @param argv the main's argv parameter.
 * @param in_path the input directory or file path buffer variable
 * @param out_dir the output directory path buffer variable
 * @param output_name_addon the name addon buffer variable (default: "_rendered")
 * @param timeout the timeout variable (default: 0)
 * @param blur_filter_size the filter size used for blur (see blur_pixel function)
 * @param verbose whether or not text has to be printed
 * @param respect_input_path whether or not the pictures in the input directory will have a similar path in the output directory.
 * @param counry_code the country code for the plate detection: "eu", "us", "au", "br", "fr", "gb", "in". Default: "eu"
 * @param save_plate_info whether or not the plate's information have to be saved
 * @param plate_info_save_path the path to file or directory to save plate information.
 * @param blur_only whether or not only bluring should be done. Works only for one file.
 * @param blur_only_location the location of the areas to be blurred. "x11_y11_x12_y12_...x21_y21...".
 * 	      The order is TOP_LEFT, TOP_RIGHT_ BOTTOM_RIGTH, BOTTOM LEFT. Example: 100_150_200_145_250_213_145_200. There must be a multiple of 8 values.
 * @param replace_input_file whether or not you want to replace intput file by the blurred image. A copy of the origin file is made with _origin.
 */
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
	bool &replace_input_file);

#endif