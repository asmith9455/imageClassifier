#ifndef POINT2_H
#define POINT2_H

template<typename T>
class Point2
{
public:
    Point2() {}
    Point2(T _x, T _y):x(_x), y(_y) {}

    T x;
    T y;
};

#endif // POINT2_H
