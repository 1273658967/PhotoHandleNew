//
// Created by eg on 2020/6/12.
#ifndef _UN_DISTORT_IMG_H_
#define _UN_DISTORT_IMG_H_

#include <iostream>
#include <opencv2/opencv.hpp>
#include<opencv2/stitching.hpp>

//#include "stitch.h"

using namespace cv;
using namespace std;

#define IMG_WIDTH 2048//1600//1280//
#define IMG_HEIGHT 1536//1200//960//

bool  Imgundistort(Mat distortimg, Mat& undistortimg, int dstwidth, int dstheight);
bool getEulerangle(Mat img, vector<Point3d> point3D, vector<Point2d> image_points, Vec3f& euler, float&dist);

#endif