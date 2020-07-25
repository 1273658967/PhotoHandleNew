#ifndef _FIND_CORNERS_H_
#define _FIND_CORNERS_H_

#include <iostream>
#include <opencv2/opencv.hpp>
#include "common.h"

using namespace cv;
using namespace std;

bool extractcorners(Mat src, vector<Point2f>& image_points_buf);
Mat rotateimg(Mat src, float angle);
float calcrotateangle(vector<Point2f> image_points_buf);
#endif