#include "utils.hpp"
#include "blur.hpp"

#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>

#include <cstdlib>
#include <iostream>
#include <assert.h>
#include <cstring>
#include <stack>

using namespace std;
using namespace cv;

#define BORDER_MARGIN 15

void blur_pixel(Mat picture, unsigned int filter_size, int i, int j, int k){
    int top = _max((int) (i - filter_size/2), 0);
    int bot = _min((int) (i + filter_size/2), picture.rows);
    int left = _max((int) (j - filter_size/2), 0);
    int right = _min((int) (j + filter_size/2), picture.cols);

    if (bot - top == 0 || right - left == 0){
        DISPLAY_ERR("Invald corners.");
        return;
    }

    unsigned int _sum = 0;
    unsigned char *data = (unsigned char*)(picture.data);
    for (int _i = top; _i < bot; _i++){
        for(int _j = left; _j < right; _j++){
            _sum += data[picture.step*_i + _j*picture.channels() + k];
        }
    }
    data[picture.step*i + j*picture.channels() + k]
        = (unsigned char) (_sum/((bot - top)*(right - left)));
}

bool in_area(int i, int j, const vector<Point> &corners){
    // j <=> x (col) and i <=> y (row)
    Point pt_tl = corners[0];
    Point pt_br = corners[1];
    Point pt_bl = corners[2];
    Point pt_tr = corners[3];

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

void blur_margin(const Mat picture, const vector<Point> &corners, unsigned int filter_size){ //TODO
    // j <=> x (col) and i <=> y (row)
    /* Point pt_tl = corners[0];
    Point pt_br = corners[1];
    Point pt_bl = corners[2];
    Point pt_tr = corners[3]; */

    //TODO: Blur le cadre avec un filtre de moins en moins puissant sur genre 10 pixels

    /* double slope;
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
    } */
}

//corners: [TOPLEFT, BOTRIGHT, BOTLEFT, TOPRIGHT]
Mat blur(const Mat picture, const vector<Point> &corners,
         unsigned int filter_size = 45){
    assert(corners.size() == 4);
    assert((int) filter_size < _min(picture.rows, picture.cols));

    // Getting corners
    Point pt_tl   = corners[0];
    Point pt_br   = corners[1];
    Point pt_bl  = corners[2];
    Point pt_tr = corners[3];
    int top   = _min(pt_tl.y, pt_tr.y);
    int bot   = _max(pt_br.y, pt_bl.y);
    int left  = _min(pt_tl.x, pt_bl.x);
    int right = _max(pt_tr.x, pt_br.x);
    if (top >= bot || left >= right || bot >= picture.rows || right >= picture.cols){
        DISPLAY_ERR("Invalid corners.");
        return Mat(); //Empty matrix
    }

    // Copying picture and blur area
    Mat blured = picture.clone();
    for (int i = top; i < bot; i++){
        for(int j = left; j < right; j++){
            for(int k = 0; k < 3; k++){
                if (in_area(i, j, corners)){
                    blur_pixel(blured, filter_size, i, j, k);
                }
            }
        }
    }

    blur_margin(blured, corners, filter_size);

    return blured;
}