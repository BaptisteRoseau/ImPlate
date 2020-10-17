#ifndef BLUR_HPP
#define BLUR_HPP

#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>

#include <cstdlib>
#include <assert.h>

/**
 * @brief blur a single pixel of coordinate i, j, and color k (< 3).
 * 
 * @param picture the matrix object of the picture
 * @param blured the matrix object buffer for the blured picture
 * @param filter_size the size of the square which pixel's mean value is used for the blur effect. (default: 60)
 * @param i row index
 * @param j col index
 * @param k color value (0, 1 or 2)
 */
void blur_pixel(cv::Mat picture, cv::Mat blured, unsigned int filter_size, int i, int j, int k);

/**
 * @brief Whether or not a pixel is into the area formed by the 4 corners.
 * 
 * @param i row index
 * @param j col index
 * @param corners the four corner as Point(row, col)
 * @return true if the pixel is in the area
 * @return false if the pixel is out of the area
 */
bool in_area(int i, int j, const std::vector<cv::Point> &corners);

/**
 * @brief Blur an image into the area formed by the 4 corners, using filter_size for the blur power (see blur_pixel function).
 * This function does not modify the initial picture.
 * 
 * @param picture the matrix object of the original picture
 * @param blured the matrix object buffer for the blured picture
 * @param corners the four corner as Point(row, col)
 * @param filter_size the size of the square which pixel's mean value is used for the blur effect. (default: 60)
 * @return 1 if failure, 0 if success
 */
int blur(const cv::Mat picture, cv::Mat blured, const std::vector<cv::Point> &corners,
         unsigned int filter_size);

#endif