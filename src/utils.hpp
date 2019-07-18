#ifndef UTILS_HPP
#define UTILS_HPP

#include <opencv2/core/mat.hpp>

#include <filesystem>
#include <fstream>
#include <cstring>
#include <stack>

// Macro for stdout messages for verbose-only mode, and log save
#define DISPLAY(stream) if (verbose){cout << stream << endl;}\
if (save_log){log_ostream << stream << endl;}

// Macro for stderr messages for verbose-only mode, and log save
#define DISPLAY_ERR(stream) if (verbose){cerr << stream << endl;}\
if (save_log){log_ostream << stream << endl;}

#define _min(a, b) ((a) < (b) ? (a) : (b))
#define _max(a, b) ((a) > (b) ? (a) : (b))

extern bool verbose;
extern bool save_log;
extern std::ofstream log_ostream;

/**
 * @brief 
 * 
 * @param path 
 * @return true 
 * @return false 
 */

bool is_supported_file(std::filesystem::path path);

/**
 * @brief 
 * 
 * @param path 
 * @param verbose 
 * @return std::stack<std::string>* 
 */

std::stack<std::string> *list_files(const char *path);
/**
 * @brief 
 * 
 * @param s 
 * @return std::string 
 */

std::string stack_next(std::stack<std::string> *s);
/**
 * @brief 
 * 
 * @param path 
 * @return int 
 */

int build_dir(const char *path);
/**
 * @brief 
 * 
 * @param path 
 * @param verbose 
 * @return cv::Mat 
 */

cv::Mat open_picture(const std::string path);

/**
 * @brief 
 * 
 * @param s 
 * @return std::string 
 */
std::string str_normalize(std::string &s);

/**
 * @brief 
 * 
 * @param picture 
 * @param dir 
 * @param name 
 * @param verbose 
 */
void save_picture(const cv::Mat &picture, std::string dir, std::string name);

/**
 * @brief Get the filename object
 * 
 * @param filepath 
 * @return std::string 
 */
std::string get_filename(const std::string &filepath);

/**
 * @brief Get the file extension object
 * 
 * @param filepath 
 * @return std::string 
 */
std::string get_file_extension(const std::string &filepath);

#endif