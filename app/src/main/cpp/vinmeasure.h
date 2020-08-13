#ifndef  _VIN_MEASURE_H_
#define _VIN_MEASURE_H_

#include <iostream>
#include <opencv2/opencv.hpp>

#include "common.h"

using namespace cv;
using namespace std;

int VinMeasureProcess(Mat rgbImg, Mat* vinImg);
//int VinMeasureProcess(Mat rgbImg, Mat* vinImg,int* code);
#endif