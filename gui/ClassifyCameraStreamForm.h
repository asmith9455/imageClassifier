#ifndef CLASSIFYCAMERASTREAMFORM_H
#define CLASSIFYCAMERASTREAMFORM_H

#include <QDialog>
#include <ColourStatisticsAnalyzer.h>
#include <ImageSequence.h>

#include <iostream>
#include <chrono>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <chrono>
#include <CvQt.h>
#include <QTimer>



namespace Ui {
class ClassifyCameraStreamForm;
}

class ClassifyCameraStreamForm : public QDialog
{
    Q_OBJECT

public:
    explicit ClassifyCameraStreamForm(QWidget *parent = 0);
    ~ClassifyCameraStreamForm();

signals:
    void timeout();

private slots:
    void displayVideo();

private:
    Ui::ClassifyCameraStreamForm *ui;
    void startClassify();
    //void displayVideo();
    QTimer* timer;
    const char* gst = "nvcamerasrc ! video/x-raw(memory:NVMM), width=(int)2592, height=(int)1458, format=(string)I420, framerate=(fraction)30/1 ! nvvidconv flip-method=2 ! video/x-raw, format=(string)I420 ! videoconvert ! video/x-raw, format=(string)BGR ! appsink";
    //const char* gst =  "nvcamerasrc ! video/x-raw(memory:NVMM), width=(int)1280, height=(int)720, format=(string)I420, framerate=(fraction)120/1 ! nvvidconv flip-method=2 ! video/x-raw, format=(string)I420 ! videoconvert ! video/x-raw, format=(string)BGR ! appsink";
    VideoCapture cap;
    cv::Mat img;
    ImageClassifier* ic;
    ColourStatisticsAnalyzer* csa;
};

#endif // CLASSIFYCAMERASTREAMFORM_H
