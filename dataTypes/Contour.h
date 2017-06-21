#ifndef CONTOUR_H
#define CONTOUR_H

#include <vector>
#include <Point2.h>

template<typename T>
class Contour
{
protected:
    std::vector<Point2<T>> points;

public:
    Contour(std::vector<Point2<T>>  _points):points(_points)
    {
    }
};

#endif // CONTOUR_H
