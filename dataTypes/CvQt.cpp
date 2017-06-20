#include "CvQt.h"

CvQt::CvQt()
{

}

QPixMap mat_2_qpixmap(cv::Mat img)
{
    return QPixmap::fromImage(QImage(img.data, img.cols, img.rows, img.step, QImage::Format_RGB888))
}
