#pragma once
// Library
#include <iostream>
#include <string>
#include <vector>
#include <math.h>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
// Self
#include "generalFunc.h"

// Find Index of Rectangle with Max Area
int maxRectAreaIndex(std::vector<cv::Rect> vect);

// Single Camera Distance Calculations
double f_len_derv(double, double, double);
double objDetectDist(double, double, double);

// Round Number for Display
double rndDoubleDown(double num, int places);

// Compute Largest Contour's Distance from Camera (with option to draw bounding rectangle and distance in meters on image)
double getMaxRectDist(cv::Mat binImg, cv::Mat& outImg, double objWidth, double foclen, int tilesize, bool drawRect, bool drawDist);

// Draw Bounding Box and Distance of Largest Contour from Camera on outImg
void drawMaxRectDist(cv::Mat binImg, cv::Mat& outImg, double objWidth, double foclen, int tilesize);

// Compute Largest Contour's Distance from Camera
double getMaxRectDist_NoDraw(cv::Mat binImg, double objWidth, double foclen, int tilesize);

// Draw Bounding Boxes for All Detected Contours with Numbering
void drawBoundingBoxes_wNumbers(cv::Mat binImg, cv::Mat& outImg, double objWidth, double foclen, int tilesize);


// DEPRECIATED
//// Structures
//struct resolution
//{
//	int width;
//	int height;
//};
//
//// Top Level Function Call
//double get_objDist(cv::Mat, double, double);
//
//// Image Processing for Pixel Width of Object Functions
//double detect_rectWidth_contour(cv::Mat im);