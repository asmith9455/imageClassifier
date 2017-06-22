#ifndef SEGMENTEDIMAGE_H
#define SEGMENTEDIMAGE_H

#include <SegmentedRegion.h>
#include <opencv2/core/core.hpp>
#include <vector>
#include <string>

template<typename T>
class SegmentedImage
{
protected:

public:
    int imgID, captureDeviceID;
    cv::Mat img;
    std::vector<SegmentedRegion<T>> segmentedRegions;
    SegmentedImage(){}
};

#endif // SEGMENTEDIMAGE_H
