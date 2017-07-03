#ifndef HSVHISTOGRAM_H
#define HSVHISTOGRAM_H

#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <exception>
#include "ImageSequence.h"
#include <fstream>
#include <iostream>

template<int channelDepth>
class HsvHistogram
{
    typedef cv::Point3_<uint8_t> Pixel;

    void fillAccumulators(cv::Mat img);

public:

    int accH[channelDepth] = { 0 };
    int accS[channelDepth] = { 0 };
    int accV[channelDepth] = { 0 };

    HsvHistogram(){}
    HsvHistogram(cv::Mat img);
    HsvHistogram(int _accH[], int _accS[], int _accV[]);

    static vector<HsvHistogram> getHistograms(ImageSequence imgSeq);
    static vector<HsvHistogram> averageHsvHistogramSequence(vector<HsvHistogram>);
    int similarityH(h1, RgbHistogram h2);
    static int similarityS(RgbHistogram h1, RgbHistogram h2);
    static int similarityB(RgbHistogram h1, RgbHistogram h2);
};
#endif // HSVHISTOGRAM_H
