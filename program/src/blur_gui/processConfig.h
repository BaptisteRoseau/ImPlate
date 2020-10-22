#ifndef PROCESS_CONFIG_H
#define PROCESS_CONFIG_H

#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/core/mat.hpp>
#include <alpr.h>

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <stack>
#include <set>

/**
 * @brief This class is the main process of the program
 * 
 */
class ProcessConfig {
public:
    ProcessConfig(char *argv[]);
    ~ProcessConfig();
    
    void init(void);
    void finalize(void);

    bool isFirstImage(void);
    bool isLastImage(void);
    bool isPictureStateChanged(void);
    cv::Mat getblurredPicture(void);
    std::string getFilepath(void);
    int currentPictureIdx(void);
    int maximumPictureIdx(void);

    void setCornersVector(std::vector<cv::Point> v);
    void clearCornersVector(void);
    void clearAutoblurPlateInfo(void);

    int firstImage(void);
    int previousImage(void);
    int nextImage(void);
    int blurImage(std::vector<std::vector<cv::Point> >);
    int saveImage(void);
    int cancel(void);
    int autoBlur(void);

    
private:
    // Command line arguments
    char* in_path;
    char* out_path;
    char *output_name_addon;
    unsigned int blur_filter_size;
    bool respect_input_path;
    char *log_file;
    char *country;
    bool save_plate_info;
    char *plate_info_save_path;
    bool replace_input_file;

    // Main process required variables
    std::stack<std::string> *stack_files, *stack_files_done;
    std::set<std::string> *success_pictures, *failed_pictures;
	cv::Mat picture, blurred;
	std::string filepath, filename, fileext, savedir;
	unsigned int loop_idx, nb_files;
    std::vector<cv::Point> corners;
    alpr::Alpr *detector;
    alpr::AlprResults alpr_results;

    // Methods
    int updatePathAndPicture(void);
};

#endif // PROCESS_CONFIG_H
