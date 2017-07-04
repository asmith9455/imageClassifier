#ifndef CONVERTERMETHODS_H
#define CONVERTERMETHODS_H

#include <QString>
#include <Point2.h>
#include <vector>
#include <Contour.h>
#include <ImageSequence.h>
#include <SegmentedRegion.h>
#include <TextureClassifier.h>
#include <ColourStatisticsAnalyzerRgb.h>
#include <CSA_RGB_WN_v1.h>
#include <memory>
#include <tinyxml2.h>

class ConverterMethods
{
public:
    ConverterMethods();
    static QString getStringFromContour(std::vector<Point2<int>> vec);
    static QString getStringFromContourVector(std::vector<std::vector<Point2<int>>> vec);
    static std::vector<Point2<int>> getContourFromString(QString qstr);
    static std::vector<std::vector<Point2<int>>> getContourVectorFromString(QString qstr);

    static Contour<int> getContourClassFromString(QString qstr);
    static std::vector<Contour<int>> getContourClassVectorFromString(QString qstr);

    template<typename T>
    static ImageSequence getImageSequenceFromSegmentedRegion(
            cv::Mat img, SegmentedRegion<T> segRgn, int tileWidth, int tileHeight)
    {
        //segmentedRegion contains 1 outer contour (representing a polygon that bounds an area)
        //and a list of inner contours (representing polygons that represent holes)
        ImageSequence imSeq;

        int xPos = 0, yPos = 0;

        while (true)
        {
            while (true)
            {
                //this is the subimage to add to the image sequence
                cv::Mat subImg = img(cv::Range(yPos, yPos + tileHeight), cv::Range(xPos, xPos + tileWidth));

                bool contained = segRgn.containsRectangle(
                            Point2<int>(xPos, yPos),
                            Point2<int>(xPos+tileWidth, yPos+tileHeight));

                if (contained)
                    imSeq.addImg(subImg);

                xPos = xPos + tileWidth;
                if (xPos + tileWidth > img.cols)
                    break;
            }
            xPos = 0;
            yPos = yPos + tileHeight;
            if (yPos + tileHeight > img.rows)
                break;
        }
        return imSeq;
    }

    static std::shared_ptr<TextureClassifier> getClassifierFromFile(std::string filepath);

};

#endif // CONVERTERMETHODS_H
