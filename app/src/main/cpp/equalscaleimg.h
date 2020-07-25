#ifndef _EQUAL_SCALE_IMG_H_
#define _EQUAL_SCALE_IMG_H_

#include <iostream>
#include <opencv2/opencv.hpp>
#include "common.h"

using namespace cv;
using namespace std;

#define A4WIDTH 3507.867
#define A4HEIGHT 2480.31

void getmm2pixel(vector<Point2f> image_points_buf, double& mm2pixel_w,
                 double& mm2pixel_h, float& chess_width, float& chess_height);

void getvincodeimg(Mat img_rotate, Rect roi, double mm2pixel_w, double mm2pixel_h, Mat& A4_Img, float chess_width, float chess_height);

#endif