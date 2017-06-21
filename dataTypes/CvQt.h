#ifndef CVQT_H
#define CVQT_H

#include <opencv2/core/core.hpp>
#include <QPixmap>

class CvQt
{
public:
    CvQt();
    static QPixmap mat_2_qpixmap_rgb888(cv::Mat img);
    static QByteArray mat_2_qbytearray(const cv::Mat &image);
    static cv::Mat qbytearray_2_mat(const QByteArray & byteArray);
};

#endif // CVQT_H
