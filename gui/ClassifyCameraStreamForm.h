#ifndef CLASSIFYCAMERASTREAMFORM_H
#define CLASSIFYCAMERASTREAMFORM_H

#include <QDialog>
#include <ColourStatisticsAnalyzerRgb.h>
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

#include <QString>
#include <QStringList>
#include <QFileDialog>
#include <memory>
#include <TextureClassifier.h>
#include <ConverterMethods.h>
#include <QMessageBox>
#include <QSizePolicy>

#include <stop_marker_detection.h>


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
    void grabVideoFrame();
    void changeImageFromDisk();

    void on_btn_startClassify_clicked();

    void on_btn_selectTrainingData_clicked();

    void on_btn_selectImagesFromDisk_clicked();

    void on_btn_pauseClassify_clicked();

    void on_btn_goBackOneImage_clicked();

    void on_btn_continueClassify_clicked();

    void on_btn_goForwardOneImage_clicked();

    void on_btn_saveCurrentImageToDisk_clicked();

private:
    enum ClassifyMode
    {
        FROM_CAMERA,FROM_DISK_IMAGES, NOT_CLASSIFYING
    };

    ClassifyMode currentMode = NOT_CLASSIFYING;

    Ui::ClassifyCameraStreamForm *ui;
    int currentImageFromDisk;
    QStringList imagesFromDisk;
    void startClassifyFromDisk();
    void determineBackAndForwardBtnStates();
    void classifyAndDisplayImage();
    void startClassifyFromCameraStream(const char* streamSetting);
    //void displayVideo();
    QTimer* timer;
    bool timerInitialized = false;
    const char* nvCam2592x1458 = "nvcamerasrc ! video/x-raw(memory:NVMM), width=(int)2592, height=(int)1458, format=(string)I420, framerate=(fraction)30/1 ! nvvidconv flip-method=2 ! video/x-raw, format=(string)I420 ! videoconvert ! video/x-raw, format=(string)BGR ! appsink";
    const char* nvCam1280x720 =  "nvcamerasrc ! video/x-raw(memory:NVMM), width=(int)1280, height=(int)720, format=(string)I420, framerate=(fraction)120/1 ! nvvidconv flip-method=2 ! video/x-raw, format=(string)I420 ! videoconvert ! video/x-raw, format=(string)BGR ! appsink";
    VideoCapture cap;
    cv::Mat img;
    cv::Mat imageToClassify;
    std::shared_ptr<TextureClassifier> ic;
    bool csaInitialized = false;
    ColourStatisticsAnalyzerRgb* csa;
    double dif;
    std::chrono::high_resolution_clock::time_point frameTime1, frameTime2, frameTime2b, frameTime3, frameTime4, frameTime5, frameTime6, frameTime7;

    TextureClassifier::ClassifiedImage classImg, classImgPP;


};

#endif // CLASSIFYCAMERASTREAMFORM_H
