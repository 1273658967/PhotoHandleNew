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

#define IMG_WIDTH 1600//2048
#define IMG_HEIGHT 1200//1536

bool  Imgundistort(Mat distortimg, Mat& undistortimg, int dstwidth, int dstheight);
bool getEulerangle(Mat img, vector<Point3d> point3D, vector<Point2d> image_points, Vec3f& euler, float&dist);

#endif