#include <ClassifyCameraStreamForm.h>
#include "ui_ClassifyCameraStreamForm.h"

ClassifyCameraStreamForm::ClassifyCameraStreamForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ClassifyCameraStreamForm)
{
    ui->setupUi(this);
    startClassify();
}

ClassifyCameraStreamForm::~ClassifyCameraStreamForm()
{
    delete ui;
    if (timerInitialized)
        delete timer;
    if (csaInitialized)
        delete csa;
}


void ClassifyCameraStreamForm::startClassify()
{
#ifdef TX1



    csa->analyze();
    cout << "Done." << std::endl;

    ic = static_cast<ImageClassifier*>(csa);

    string name;

    cout << "Starting image classification loop... ";

    string imageToTest = "/home/ubuntu/image_database/image_database_parkingLot/galaxys7/test_img_0000.jpg";

    //string imageToTest = "C:\\Users\\Joe\\OneDrive\\Capstone 2017\\image_database_parkingLot\\galaxys7\\test_img_0000.jpg";



    cap = VideoCapture(gst);

    //these options do not improve latency
    //cap.set(CV_CAP_PROP_BUFFERSIZE, 3);
    //cap.set(CV_CAP_PROP_FPS, 30);
    timerInitialized = true;
    timer = new QTimer(this);

    connect(timer, SIGNAL(timeout()), this, SLOT(displayVideo()));
    timer->start(300);

    return;

#endif
}

void ClassifyCameraStreamForm::displayVideo()
{

    frameTime1 = std::chrono::high_resolution_clock::now();

    cap >> img; // get a new frame from camera

    frameTime2 = std::chrono::high_resolution_clock::now();

    TextureClassifier::ClassifiedImage classImg = ic->classifyImage(img, true, true);

    frameTime3 = std::chrono::high_resolution_clock::now();

    TextureClassifier::ClassifiedImage classImgPP = ic->postProcessImage(classImg);

    frameTime4 = std::chrono::high_resolution_clock::now();


    cv::cvtColor(img, img, CV_BGR2RGB);
    cv::cvtColor(classImg.colouredImage, classImg.colouredImage, CV_BGR2RGB);
    cv::cvtColor(classImgPP.colouredImage, classImgPP.colouredImage, CV_BGR2RGB);
    cv::cvtColor(classImgPP.binaryImageMat, classImgPP.binaryImageMat, CV_GRAY2RGB);

    frameTime5 = std::chrono::high_resolution_clock::now();
    //cv::imshow()
    ui->label_originalImage->setPixmap(QPixmap::fromImage(QImage(img.data, img.cols, img.rows, img.step, QImage::Format_RGB888)));
    ui->label_classifiedImagePpBinary->setPixmap(QPixmap::fromImage(QImage(classImgPP.binaryImageMat.data, classImgPP.binaryImageMat.cols, classImgPP.binaryImageMat.rows, classImgPP.binaryImageMat.step, QImage::Format_RGB888)));
    ui->label_classifiedImagePpRecoloured->setPixmap(QPixmap::fromImage(QImage(classImgPP.colouredImage.data, classImgPP.colouredImage.cols, classImgPP.colouredImage.rows, classImgPP.colouredImage.step, QImage::Format_RGB888)));
    ui->label_classifiedImageRecoloured->setPixmap(QPixmap::fromImage(QImage(classImg.colouredImage.data, classImg.colouredImage.cols, classImg.colouredImage.rows, classImg.colouredImage.step, QImage::Format_RGB888)));
    frameTime6 = std::chrono::high_resolution_clock::now();
    ui->label_processingFrameRate->setText("Processing Time: "+QString::number(std::chrono::duration_cast<std::chrono::milliseconds>(frameTime6 - frameTime1).count()) + " ms");
    ui->label_timeToGrabFrame->setText("Time to Grab Frame: "+QString::number(std::chrono::duration_cast<std::chrono::milliseconds>(frameTime2 - frameTime1).count()) + " ms");
    ui->label_timeToClassify->setText("Time to Classify: "+QString::number(std::chrono::duration_cast<std::chrono::milliseconds>(frameTime3 - frameTime2).count()) + " ms");
    ui->label_timeToPostProcess->setText("Time to PostProcess: "+QString::number(std::chrono::duration_cast<std::chrono::milliseconds>(frameTime4 - frameTime3).count()) + " ms");
    ui->label_timeToConvert->setText("Time to Convert: "+QString::number(std::chrono::duration_cast<std::chrono::milliseconds>(frameTime5 - frameTime4).count()) + " ms");
    ui->label_timeToDisplay->setText("Time to Display: "+QString::number(std::chrono::duration_cast<std::chrono::milliseconds>(frameTime6 - frameTime5).count()) + " ms");
}
