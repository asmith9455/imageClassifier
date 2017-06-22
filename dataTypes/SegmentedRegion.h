#ifndef SEGMENTEDREGION_H
#define SEGMENTEDREGION_H

#include <Contour.h>

template<typename T>
class SegmentedRegion
{

protected:
    Contour<T> outerContour;
    std::vector<Contour<T>> innerContours;

public:
    int imgID, segmentedRegionID;
    std::vector<int> propertyIDs;
    SegmentedRegion(){}

};

#endif // SEGMENTEDREGION_H
