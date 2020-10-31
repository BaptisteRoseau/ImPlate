#ifndef UTILS_HPP
#define UTILS_HPP

#include <opencv4/opencv2/core/mat.hpp>
#include <alpr.h>

#include <filesystem>
#include <fstream>
#include <cstring>
#include <stack>
#include <cstdlib>

//TODO: add FILE and LINE into errors

// Macro for stdout messages for verbose-only mode, and log save
#define DISPLAY(stream) if (verbose){cout << stream << endl;}\
if (save_log){log_ostream << stream << endl;}

// Macro for stderr messages for verbose-only mode, and log save
#define DISPLAY_ERR(stream) if (verbose){cerr << "\033[1;31mERROR:\033[0m " << stream << endl;}\
if (save_log){log_ostream << "ERROR: " << stream << endl;}

// Macro for stderr messages for verbose-only mode, and log save
#define DISPLAY_WAR(stream) if (verbose){cerr << "\033[1;35mWARNING:\033[0m " << stream << endl;}\
if (save_log){log_ostream << "WARNING: " << stream << endl;}

// Macro for stderr messages for critical errors. Exits after prompt.
#define DISPLAY_ERR_EXIT(stream) cerr << "\033[1;31mERROR:\033[0m " << stream << endl;}\
if (save_log){log_ostream << "ERROR: " << stream <<                                    \
" at file " << __FILE__ << " line " << __LINE__ << endl;                               \
exit(EXIT_FAILURE);

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
 * @brief Leave only the last dot and replace the others with "_"
 * 
 * @param str A path to a file as string
 * @return std::string normalized by '_' except the last dot and its following
 */
std::string str_normalize_except_last_dot(std::string str);

/**
 * @brief List every files in the given file/directory path recursively.
 * Results will be put into a stack.
 * 
 * @param path is the path to the file or directory to compute
 * @return std::stack<std::string>* contains the path to all the files to compute
 */
std::stack<std::string> *list_files(const char *path);

/**
 * @brief Copy top of s1 onto s2.
 * 
 * @param s1 Non-empty stack
 * @param s2 Stack that will receive the top of s1
 * @return false for failure, true for success
 */
bool copy_top(std::stack<std::string> *s1, std::stack<std::string> *s2);

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
 * @brief Replace dots '.' by '_'
 * 
 * @param str the input string
 * @return std::string copy of the input having '.' replaced by '_'
 */
std::string remove_dots(const std::string str);

/**
 * @brief Replace every space, (, ), [ and ] by _
 * This function modify it's argument.
 * 
 * @param str the string to normalize.
 * @return std::string a copy of input, with spaces, (, ), [ and ] replaced by _.
 */
std::string str_normalize(const std::string str);

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
 * @brief Rename file or empty directory.
 * 
 * @param from is the source path
 * @param to is the destination path
 */
void rename(std::string from, std::string to);

/**
 * @brief Remove a file or an empty directory.
 * 
 * @param path is the path to the file or the empty directory
 */
void remove(std::string path);

/**
 * @brief Recursuvely removes empty directories from path to its first non-empty parent (not included)
 * 
 * @param path is the path to an empty directory
 */
void remove_empty_directories(std::string path);

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

/**
 * @brief Retrieves the plate(s) corners and number from results to corners and numbers
 * 
 * @param results the returned value of the alpr process
 * @param corners a buffer where to write corners (from top-left following the chronological order)
 * @param numbers a buffer where to write the detected plates
 */
void plate_corners(const std::vector<alpr::AlprPlateResult> &results,
				 std::vector<std::vector<cv::Point> > &corners,
				 std::vector<std::string> &numbers);

#endif