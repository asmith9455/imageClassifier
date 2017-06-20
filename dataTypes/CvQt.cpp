#include "CvQt.h"

CvQt::CvQt()
{

}

QPixmap mat_2_qpixmap_rgb888(cv::Mat img)
{
    return QPixmap::fromImage(QImage(img.data, img.cols, img.rows, img.step, QImage::Format_RGB888));
}
