#ifndef UTILS_HPP
#define UTILS_HPP

#include <opencv4/opencv2/core/mat.hpp>

#include <filesystem>
#include <fstream>
#include <cstring>
#include <stack>
#include <cstdlib>

// Macro for stdout messages for verbose-only mode, and log save
#define DISPLAY(stream) if (verbose){cout << stream << endl;}\
if (save_log){log_ostream << stream << endl;}

// Macro for stderr messages for verbose-only mode, and log save
#define DISPLAY_ERR(stream) if (verbose){cerr << "\033[1;31mERROR:\033[0m " << stream << endl;}\
if (save_log){log_ostream << "ERROR: " << stream << endl;}

// Macro for stderr messages for verbose-only mode, and log save
#define DISPLAY_WAR(stream) if (verbose){cerr << "\033[1;35mWARNING:\033[0m " << stream << endl;}\
if (save_log){log_ostream << "WARNING: " << stream << endl;}

#define _min(a, b) ((a) < (b) ? (a) : (b))
#define _max(a, b) ((a) > (b) ? (a) : (b))

extern bool verbose;
extern bool save_log;
extern std::ofstream log_ostream;

/**
 * @brief Verify if the file's extension is a supported format.
 * 
 * @param path is the path to the file.
 * @return true if the format is suported
 * @return false else
 */

bool is_supported_file(std::filesystem::path path);

/**
 * @brief List every files in the given file/directory path recursively.
 * Results will be put into a stack.
 * 
 * @param path is the path to the file or directory to compute
 * @return std::stack<std::string>* contains the path to all the files to compute
 */
std::stack<std::string> *list_files(const char *path);

/**
 * @brief return the top element of the stack, then pop it
 * 
 * @param s the string stack
 * @return std::string the top of the stack, before poping
 */
std::string stack_next(std::stack<std::string> *s);

/**
 * @brief builds a directory for the given path.
 * If the path refers to a file, this will return an error.
 * 
 * @param path to the directory to create.
 * @return false for failure, true for success
 */
bool build_dir(const char *path);

/**
 * @brief Open a picture with OpenCV
 * 
 * @param path the path to the picture to open.
 * @return cv::Mat the Mat object containing picture. This Mat will be empty if an error occurs.
 */
cv::Mat open_picture(const std::string path);

/**
 * @brief Replace every space, (, ), [ and ] by _
 * This function modify it's argument.
 * 
 * @param s the string to normalize.
 * @return std::string same as argument.
 */
std::string str_normalize(std::string &s);

/**
 * @brief Writes the picture from Mat object into the "dir/name" file.
 * 
 * @param picture is the matrix object to save.
 * @param dir is the directory where the picture has to be saved.
 * @param name is the name of the file to save.
 */
void save_picture(const cv::Mat &picture, std::string dir, std::string name);

/**
 * @brief Writes the picture from Mat object into the "path" file.
 * 
 * @param picture is the matrix object to save.
 * @param path is the path where the picture has to be saved
 */
void save_picture(const cv::Mat &picture, std::string path);

/**
 * @brief Get the name of the file without path nor extension.
 * 
 * @param filepath the path to the file.
 * @return std::string the name of the file, without extension.
 */
std::string get_filename(const std::string &filepath);

/**
 * @brief Get the file's extension
 * 
 * @param filepath the path to the file
 * @return std::string the extension (with the dot).
 */
std::string get_file_extension(const std::string &filepath);

/**
 * @brief Selects which output directory will be choosen, according to respect_input_path.
 * If respect_input_path is set to true, the path will be similar as the one into the input directory, plus the filename as directory.
 * If the respect_input_path is set to false, the directory will have the same name as the filename.
 * The output root is out_dir.
 * If the input file is a picture, the behavior will be the same as with respect_input_path set to false.
 * 
 * @param out_dir the output root directory
 * @param filepath the path of the current file
 * @param respect_input_path is wether or not the input directory's path must be copied or not.
 * @return string the path to the final output directory
 */
std::string select_output_dir(const std::string out_dir, const std::string in_path, const std::string filepath, const bool respect_input_path);

/**
 * @brief Recursively build requiered directories for path.
 * If the input is a file, only the requiered directories will be built
 * 
 * @param path the path to the output directory.
 */
bool build_directories(const std::string path);

/**
 * @brief Add the addon string before the extension of the file
 * 
 * @param path the path of the file.
 * @param addon the addon to add before the extension.
 */
std::string add_addon(std::string path, std::string addon);


void sort_corners(std::vector<cv::Point> &corners);

/**
 * @brief Recursively build requiered directories for path.
 * 
 * @param str_location is the input from the command line.
 *  It must respect format "x11_y11_x12_y12_x13_y13_x14_y14_x21_y21...".
 *  Every multiple of 8 values is an area to blur formed by 4 points (x, y).
 *  The order for the points is TOP_LEFT, TOP_RIGHT, BOTTOM_RIGHT, BOTTOM_LEFT
 * @return a vector having the good format to be used by blur function.
 */
std::vector<std::vector<cv::Point> > parse_location(const std::string str_location);

#endif