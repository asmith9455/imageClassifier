#pragma once
#include <ColourStatisticsAnalyzer.h>
#include <ImageSequence.h>

#include <iostream>
#include <chrono>
#include <string>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>

class ProgramManager
{
public:
	//static void classifyDisplaySingleImage(std::string imagePath);
	static void trainClassifyDisplaySingleImage();
	static void trainClassifyDisplayCameraStream();
};
