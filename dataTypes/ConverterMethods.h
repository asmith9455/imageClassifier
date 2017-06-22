#ifndef CONVERTERMETHODS_H
#define CONVERTERMETHODS_H

#include <QString>
#include <Point2.h>
#include <vector>
#include <Contour.h>

class ConverterMethods
{
public:
    ConverterMethods();
    static QString getStringFromContour(std::vector<Point2<int>> vec);
    static QString getStringFromContourVector(std::vector<std::vector<Point2<int>>> vec);
    static std::vector<Point2<int>> getContourFromString(QString qstr);
    static std::vector<std::vector<Point2<int>>> getContourVectorFromString(QString qstr);

    static Contour<int> getContourClassFromString(QString qstr);
    static std::vector<Contour<int>> ConverterMethods::getContourClassVectorFromString(QString qstr);
};

#endif // CONVERTERMETHODS_H
