#include "ConverterMethods.h"

ConverterMethods::ConverterMethods()
{}


QString ConverterMethods::getStringFromContour(std::vector<Point2<int>> vec)
{
    std::vector<std::vector<Point2<int>>> tmpVec;
    tmpVec.push_back(vec);

    return getStringFromContourVector(tmpVec);
}

QString ConverterMethods::getStringFromContourVector(std::vector<std::vector<Point2<int>>> vec)
{
    QString tmp = "[";
    QString tmp2;
    for(size_t i = 0; i < vec.size(); i++)
    {

        tmp += "[";
        tmp2 = "";

        for(size_t j = 0; j < vec[i].size(); j++)
        {
            tmp2 += "[" + QString::number(vec[i][j].x) + "," + QString::number(vec[i][j].y) +"]";
        }
        tmp += tmp2;

        tmp += "]";
    }
    tmp +="]";
    return tmp;
}

std::vector<Point2<int>> ConverterMethods::getContourFromString(QString qstr)
{
    std::vector<std::vector<Point2<int>>> vec = getContourVectorFromString(qstr);
    return vec[0];
}

std::vector<std::vector<Point2<int>>> ConverterMethods::getContourVectorFromString(QString qstr)
    {
        std::vector<std::vector<Point2<int>>> vec;
        int depth = 0;
        std::string tmp = "";
        int xTmp, yTmp;

        std::string str = qstr.toStdString();

        enum mode { readingX, readingY, parsingSyms };

        mode pMode = parsingSyms;

        for(size_t i = 0; i < str.size(); i++)
        {
            char c = str.at(i);
            if (c == '[')
            {
                depth++;
                if (depth == 2)
                    vec.push_back(std::vector<Point2<int>>());
            }
            else if (c==']')
            {
                depth--;

            }
            if (depth == 0)
                break;

            if(pMode == readingX)
            {
                if(c == ',')
                {
                    xTmp = std::stoi(tmp);
                    pMode = readingY;
                    tmp = "";
                    continue;
                }
                else
                    tmp += c;
            }

            if(pMode == readingY)
            {
                if(depth == 2)
                {
                    yTmp = std::stoi(tmp);
                    vec.back().push_back(Point2<int>(xTmp, yTmp));
                    pMode = parsingSyms;
                    tmp = "";
                }
                else
                    tmp += c;
            }

            if(depth == 3 && pMode == parsingSyms)
                pMode = readingX;

        }
        return vec;
    }



Contour<int> ConverterMethods::getContourClassFromString(QString qstr)
{
    return Contour<int>(getContourFromString(qstr));
}


std::vector<Contour<int>> ConverterMethods::getContourClassVectorFromString(QString qstr)
{
    std::vector<std::vector<Point2<int>>> tmp = getContourVectorFromString(qstr);
    std::vector<Contour<int>> cs;
    for (size_t i = 0; i < tmp.size(); i++)
        cs.push_back( Contour<int>(tmp[i]) );

    return cs;
}

