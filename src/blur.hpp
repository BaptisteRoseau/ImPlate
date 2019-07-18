#ifndef BLUR_HPP
#define BLUR_HPP

#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>

#include <cstdlib>
#include <assert.h>

/**
 * @brief 
 * 
 * @param picture 
 * @param filter_size 
 * @param _i 
 * @param _j 
 * @param _k 
 */
void blur_pixel(cv::Mat picture, unsigned int filter_size, int _i, int _j, int _k);

/**
 * @brief 
 * 
 * @param i 
 * @param j 
 * @param corners 
 * @return true 
 * @return false 
 */
bool in_area(int i, int j, const std::vector<cv::Point> &corners);

/**
 * @brief 
 * 
 * @param picture 
 * @param corners 
 * @param filter_size (Optional)
 * @return cv::Mat 
 */
cv::Mat blur(const cv::Mat picture, const std::vector<cv::Point> &corners,
             unsigned int filter_size);

#endif