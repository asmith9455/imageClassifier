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
    SegmentedRegion();
};

#endif // SEGMENTEDREGION_H
