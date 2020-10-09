#include "utils.h"
#include "blur.h"
#include "config.h"

#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/core/mat.hpp>

#include <cstdlib>
#include <iostream>
#include <cstring>
#include <stack>

using namespace std;
using namespace cv;

#define BORDER_MARGIN 50

//OpenCV blur: https://www.tutorialkart.com/opencv/python/opencv-python-gaussian-image-smoothing/
void blur_pixel(Mat picture, Mat blured, unsigned int filter_size, int i, int j, int k){
    int top = _max((int) (i - filter_size/2), 0);
    int bot = _min((int) (i + filter_size/2), picture.rows);
    int left = _max((int) (j - filter_size/2), 0);
    int right = _min((int) (j + filter_size/2), picture.cols);

    if (bot - top == 0 || right - left == 0){
        DISPLAY_ERR("Invald corners.");
        return;
    }

    // Getting submatrix for filter calculation
    Range r_x = Range(top, bot);
    Range r_y = Range(left, right);
    Mat submat = picture.operator()(r_x, r_y);

    // Submatrix sum calculation
    Scalar sum_vector = sum(submat);
    blured.data[picture.step*i + j*picture.channels() + k]
        = (unsigned char) (sum_vector[k] / (submat.rows*submat.cols));
}

bool in_area(int i, int j, const vector<Point> &corners){
    // j <=> x (col) and i <=> y (row)
    Point pt_tl = corners[0];
    Point pt_tr = corners[1];
    Point pt_br = corners[2];
    Point pt_bl = corners[3];

    double slope;
    // Left Area
    slope = ((double)(pt_tl.x - pt_bl.x))/(pt_tl.y - pt_bl.y);
    if (j < pt_bl.x + slope*(i - pt_bl.y)){
        return false;
    }

    // Top Area
    slope = ((double)(pt_tl.y - pt_tr.y))/(pt_tl.x - pt_tr.x);
    if (i < pt_tl.y + slope*(j - pt_tl.x)){
        return false;
    }

    // Right Area
    slope = ((double)(pt_tr.x - pt_br.x))/(pt_tr.y - pt_br.y);
    if (j > pt_br.x + slope*(i - pt_br.y)){
        return false;
    }

    // Bottom Area
    slope = ((double)(pt_br.y - pt_bl.y))/(pt_br.x - pt_bl.x);
    if (i > pt_bl.y + slope*(j - pt_bl.x)){
        return false;
    }

    return true;
}

//corners: [TOPLEFT, TOPRIGHT, BOTRIGHT, BOTLEFT]
int blur(const Mat picture, Mat blured, const vector<Point> &corners,
         unsigned int filter_size = 45){
    // Input verification (don't use assert as you don't want the whole program to stop)
    if (corners.size() != 4){
        DISPLAY_ERR("Invalid corner detection. Skiping picture.");
        return EXIT_FAILURE;
    }
    if ((int) filter_size >= _min(picture.rows, picture.cols)){
        DISPLAY_ERR("Filter size too large for this picture. Skiping picture.");
        return EXIT_FAILURE;
    }

    // Getting corners
    Point pt_tl = corners[0];
    Point pt_tr = corners[1];
    Point pt_br = corners[2];
    Point pt_bl = corners[3];
    int top   = _min(pt_tl.y, pt_tr.y);
    int bot   = _max(pt_br.y, pt_bl.y);
    int left  = _min(pt_tl.x, pt_bl.x);
    int right = _max(pt_tr.x, pt_br.x);
    if (top >= bot || left >= right || bot >= picture.rows || right >= picture.cols){
        DISPLAY_ERR("Invalid corners.");
        return EXIT_FAILURE;
    }

    // Copying picture and blur area
    #pragma omp parallel for collapse(2)
    for (int i = top; i < bot; i++){
        for(int j = left; j < right; j++){
            for(int k = 0; k < 3; k++){
                if (in_area(i, j, corners)){
                    blur_pixel(picture, blured, filter_size, i, j, k);
                }
            }
        }
    }

    return EXIT_SUCCESS;
}