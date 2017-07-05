#pragma once
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <exception>
#include "ImageSequence.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <vector>

using namespace cv;
using namespace std;

class RgbHistogram
{
	typedef cv::Point3_<uint8_t> Pixel;

	void fillAccumulators(Mat img);

public:

    int accR[256] = {0};
    int accG[256] = {0};
    int accB[256] = {0};

    int meanBinR, meanBinB, meanBinG;

    int numR, numG, numB;

    RgbHistogram();
	RgbHistogram(Mat img);
	RgbHistogram(int _accR[], int _accG[], int _accB[]);
	 
    void rotateMeanUsingRed(int newBinForMeanR);
    void rotateLeft(int rotateAmountLeft);
    void rotateRight(int rotateAmountRight);
    int getSumAbsDifferenceR(RgbHistogram h);
    int getSumAbsDifferenceG(RgbHistogram h);
    int getSumAbsDifferenceB(RgbHistogram h);

	static vector<RgbHistogram> getHistograms(ImageSequence imgSeq);
    static vector<RgbHistogram> averageRgbHistogramSequence(vector<RgbHistogram>);
	static void writeHistosToFile(vector<vector<RgbHistogram>>, string path);
	static void writeHistosToFile(vector<RgbHistogram>, string path);
	static int similarityR(RgbHistogram h1, RgbHistogram h2);
	static int similarityG(RgbHistogram h1, RgbHistogram h2);
	static int similarityB(RgbHistogram h1, RgbHistogram h2);

	static bool AreEqual(RgbHistogram h1, RgbHistogram h2);

	static bool ListIsEqual(vector<RgbHistogram> h1, vector<RgbHistogram> h2);
	static bool ListOfListIsEqual(vector<vector<RgbHistogram>> h1, vector<vector<RgbHistogram>> h2);
};
