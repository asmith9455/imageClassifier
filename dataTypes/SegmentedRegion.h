#ifndef SEGMENTEDREGION_H
#define SEGMENTEDREGION_H

#include <Contour.h>

template<typename T>
class SegmentedRegion
{

protected:


public:
    Contour<T> outerContour;
    std::vector<Contour<T>> innerContours;
    int imgID, segmentedRegionID;
    std::vector<int> propertyIDs;
    SegmentedRegion(){}

};

#endif // SEGMENTEDREGION_H
