#pragma once
// Library
#include <iostream>
#include <string>
#include <vector>
#include <opencv2/core/core.hpp>
//#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <stdexcept>


// Self
#include <generalFunc.h>

namespace stop_marker_detection
{

// Top Level Function Call
double get_objdist(cv::Mat, double, double);

// Image Processing for Pixel Width of Object Functions
double detect_rectWidth_contour(cv::Mat im);

// Single Camera Distance Functions
double f_len_derv(double, double, double);
double objDetectDist(double, double, double);

void detect_rectangleOnBinaryImage(cv::Mat binImg, cv::Mat& colImg);



}
