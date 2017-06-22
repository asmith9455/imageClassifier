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
    Contour(){}
    Contour(std::vector<Point2<T>>  _points):points(_points)
    {
    }

    bool contains(Point2<T> pnt)
    {
        int windings = wn_PnPoly(pnt);

        if (windings == 0)
            return false;
        else
            return true;
    }

protected:
    //algorithm and original code by Dan Sunday (http://geomalgorithms.com/a03-_inclusion.html)
    // adapted to run with our types
    // wn_PnPoly(): winding number test for a point in a polygon
    //      Input:   P = a point,
    //               V[] = vertex points of a polygon V[n+1] with V[n]=V[0]
    //      Return:  wn = the winding number (=0 only when P is outside)
    int wn_PnPoly(Point2<T> P)
    {
        int wn = 0;    // the  winding number counter

        // loop through all edges of the polygon
        for (size_t i = 0; i < points.size() - 1; i++)
        {   // edge from V[i] to  V[i+1]
            if (points[i].y <= P.y)
            {          // start y <= P.y
                if (points[i + 1].y > P.y)      // an upward crossing
                    if (isLeft(points[i], points[i + 1], P) > 0)  // P left of  edge
                        ++wn;            // have  a valid up intersect
            }
            else
            {                        // start y > P.y (no test needed)
                if (points[i + 1].y <= P.y)     // a downward crossing
                    if (isLeft(points[i], points[i + 1], P) < 0)  // P right of  edge
                        --wn;            // have  a valid down intersect
            }
        }
        return wn;
    }

    //algorithm and original code by Dan Sunday (http://geomalgorithms.com/a03-_inclusion.html)
    // adapted to run with our types
    // isLeft(): tests if a point is Left|On|Right of an infinite line.
    //    Input:  three points P0, P1, and P2
    //    Return: >0 for P2 left of the line through P0 and P1
    //            =0 for P2  on the line
    //            <0 for P2  right of the line
    //    See: Algorithm 1 "Area of Triangles and Polygons"
    double isLeft(Point2<T> P0, Point2<T> P1, Point2<T> P2)
    {
        return ((P1.x - P0.x) * (P2.y - P0.y)
                - (P2.x - P0.x) * (P1.y - P0.y));
    }

};

#endif // CONTOUR_H
