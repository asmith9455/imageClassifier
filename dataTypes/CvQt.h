#ifndef CVQT_H
#define CVQT_H

#include <opencv2/core/core.hpp>
#include <QPixmap>

class CvQt
{
public:
    CvQt();
    static QPixmap mat_2_qpixmap_rgb888(cv::Mat img);
};

#endif // CVQT_H
