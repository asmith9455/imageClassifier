#pragma once
#include <iostream>
#include <vector>
#include <time.h>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>

void delay(unsigned int);
std::vector<double> shift_right_insert(std::vector<double>, double);
cv::Mat pyrDownDef(cv::Mat, int);