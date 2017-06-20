#pragma once
#include "ColourStatisticsAnalyzer.h"
#include "ImageSequence.h"

#include <iostream>
#include <chrono>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc/imgproc.hpp>


//working towards deprecating these test functions (also want to avoid using namespaces like this)
using namespace cv;
using namespace std;
using namespace std::chrono;


int test0001();
int test0002();
int test0003();
