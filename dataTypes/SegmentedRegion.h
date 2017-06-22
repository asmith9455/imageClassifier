#ifndef SEGMENTEDREGION_H
#define SEGMENTEDREGION_H

#include <Contour.h>
#include <Point2.h>

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

    bool hasProperty(int propertyID)
    {
        bool hasIt = false;
        for(int i : propertyIDs)
            if (i == propertyID)
            {
                hasIt = true;
                break;
            }
        return hasIt;
    }

    bool contains(Point2<T> pnt)
    {
        bool outerContainsPoint = outerContour.contains(pnt);
        bool holeContainsPoint = false;
        for (Contour<T> innerContour : innerContours)
            holeContainsPoint = holeContainsPoint | innerContour.contains(pnt);

        return outerContainsPoint && !holeContainsPoint;
    }

    bool containsRectangle(Point2<T> pntSmallerXy, Point2<T> pntLargerXy)
    {
        bool containsRect = true;

        for(int x = pntSmallerXy.x; x < pntLargerXy.x; x++ )
            for(int y = pntSmallerXy.y; y < pntLargerXy.y; y++)
                containsRect = containsRect && this->contains(Point2<T>(x, y));

        return containsRect;
    }

};

#endif // SEGMENTEDREGION_H
