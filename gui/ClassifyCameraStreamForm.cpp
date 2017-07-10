#include <ClassifyCameraStreamForm.h>
#include "ui_ClassifyCameraStreamForm.h"

ClassifyCameraStreamForm::ClassifyCameraStreamForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ClassifyCameraStreamForm)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Window);
    this->showMaximized();
    //startClassify();
}

ClassifyCameraStreamForm::~ClassifyCameraStreamForm()
{
    delete ui;
    if (timerInitialized)
        delete timer;
    //if (csaInitialized)
        //delete csa;
}


void ClassifyCameraStreamForm::startClassifyFromCameraStream(const char* streamSetting)
{

    cap = VideoCapture(streamSetting);

    //these options do not improve latency
    //cap.set(CV_CAP_PROP_BUFFERSIZE, 3);
    //cap.set(CV_CAP_PROP_FPS, 30);


    if (timerInitialized)
        delete timer;

    timerInitialized = true;
    timer = new QTimer(this);

    connect(timer, SIGNAL(timeout()), this, SLOT(grabVideoFrame()));

    this->saveCaptureContinuousCount = 0;

    timer->start(300);

    return;

}

void ClassifyCameraStreamForm::startClassifyFromDisk()
{
    this->saveCaptureContinuousCount = 0;
    currentImageFromDisk = 0;
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(changeImageFromDisk()));
    timer->start(500);
}

void ClassifyCameraStreamForm::changeImageFromDisk()
{
    frameTime1 = std::chrono::high_resolution_clock::now();
    if (currentMode == FROM_DISK_IMAGES && currentImageFromDisk >= imagesFromDisk.size())
    {
        timer->stop();
        currentMode=NOT_CLASSIFYING;
        ui->btn_pauseClassify->setEnabled(false);
    }
    else if (currentMode == FROM_DISK_IMAGES)
    {
        imageToClassify = cv::imread(imagesFromDisk[currentImageFromDisk].toStdString());

        //the read image is in BGR - all of the functions work with bgr images as is the standard with opencv

        cv::cvtColor(imageToClassify, imageToClassify, CV_BGR2RGB);

        currentImageFromDisk++;
        frameTime2 = std::chrono::high_resolution_clock::now();
        classifyAndDisplayImage();
    }
}

void ClassifyCameraStreamForm::classifyAndDisplayImage()
{

    if (ui->checkBox_saveOriginalImageContinuously->isChecked())
    {
        QString fullpath = continuousFileSavePath + QString::number(saveCaptureContinuousCount) + ".bmp";
        cv::Mat toWrite;
        cv::cvtColor(imageToClassify, toWrite, cv::COLOR_BGR2RGB);
        cv::imwrite(fullpath.toStdString(), toWrite);
        saveCaptureContinuousCount++;
    }


    frameTime2b = std::chrono::high_resolution_clock::now();

    classImg = ic->classifyImage(imageToClassify, true, true);

    frameTime3 = std::chrono::high_resolution_clock::now();

    classImgPP = ic->postProcessImage(classImg);

    frameTime4 = std::chrono::high_resolution_clock::now();

    //cv::cvtColor(imageToClassify, imageToClassify, CV_BGR2RGB);
    //cv::cvtColor(classImg.colouredImage, classImg.colouredImage, CV_BGR2RGB);
    //cv::cvtColor(classImgPP.colouredImage, classImgPP.colouredImage, CV_BGR2RGB);


    frameTime5 = std::chrono::high_resolution_clock::now();

    if (ui->checkBox_drawBoundingBox->isChecked())
    {
        drawMaxRectDist(classImgPP.binaryImageMat, imageToClassify, 5.08, 96.11, ic->getTileHeight());
    }

    cv::cvtColor(classImgPP.binaryImageMat, classImgPP.binaryImageMat, CV_GRAY2RGB);

    frameTime6 = std::chrono::high_resolution_clock::now();

    int newWidth, newHeight;

    if (ui->label_originalImage->pixmap() != NULL)
        newWidth = ui->label_originalImage->pixmap()->height();
    else
        newWidth = ui->label_originalImage->width();

    if (ui->label_originalImage->pixmap() != NULL)
        newHeight = ui->label_originalImage->pixmap()->height();
    else
        newHeight = ui->label_originalImage->height();

    cv::Mat ogImg, ppBinImg, colImg, ppColImg;


    cv::resize(imageToClassify, ogImg,
               cv::Size(newWidth, newHeight), cv::INTER_AREA);
    cv::resize(classImgPP.binaryImageMat, ppBinImg,
               cv::Size(newWidth, newHeight), cv::INTER_AREA);
    cv::resize(classImg.colouredImage, colImg,
               cv::Size(newWidth, newHeight), cv::INTER_AREA);
    cv::resize(classImgPP.colouredImage, ppColImg,
               cv::Size(newWidth, newHeight), cv::INTER_AREA);



    ui->label_originalImage->setPixmap(QPixmap::fromImage(QImage(ogImg.data, ogImg.cols, ogImg.rows, ogImg.step, QImage::Format_RGB888)));
    ui->label_classifiedImagePpBinary->setPixmap(QPixmap::fromImage(QImage(ppBinImg.data, ppBinImg.cols, ppBinImg.rows, ppBinImg.step, QImage::Format_RGB888)));
    ui->label_classifiedImagePpRecoloured->setPixmap(QPixmap::fromImage(QImage(ppColImg.data, ppColImg.cols, ppColImg.rows, ppColImg.step, QImage::Format_RGB888)));
    ui->label_classifiedImageRecoloured->setPixmap(QPixmap::fromImage(QImage(colImg.data, colImg.cols, colImg.rows, colImg.step, QImage::Format_RGB888)));

    //ui->label_originalImage->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    frameTime7 = std::chrono::high_resolution_clock::now();



    ui->label_processingFrameRate->setText("Processing Time: "+QString::number(std::chrono::duration_cast<std::chrono::milliseconds>(frameTime7 - frameTime1).count()) + " ms");
    ui->label_timeToGrabFrame->setText("Time to Grab Frame: "+QString::number(std::chrono::duration_cast<std::chrono::milliseconds>(frameTime2 - frameTime1).count()) + " ms");
    ui->label_timeToClassify->setText("Time to Classify: "+QString::number(std::chrono::duration_cast<std::chrono::milliseconds>(frameTime3 - frameTime2b).count()) + " ms");
    ui->label_timeToPostProcess->setText("Time to PostProcess: "+QString::number(std::chrono::duration_cast<std::chrono::milliseconds>(frameTime4 - frameTime3).count()) + " ms");
    ui->label_timeToConvert->setText("Time to Convert: "+QString::number(std::chrono::duration_cast<std::chrono::milliseconds>(frameTime5 - frameTime4).count()) + " ms");
    ui->label_timeToDraw->setText("Time to Draw: " + QString::number(std::chrono::duration_cast<std::chrono::milliseconds>(frameTime6 - frameTime5).count()) + " ms");
    ui->label_timeToDisplay->setText("Time to Display: "+QString::number(std::chrono::duration_cast<std::chrono::milliseconds>(frameTime7 - frameTime6).count()) + " ms");
}

void ClassifyCameraStreamForm::grabVideoFrame()
{

    frameTime1 = std::chrono::high_resolution_clock::now();

    cap >> imageToClassify; // get a new frame from camera

    cv::cvtColor(imageToClassify, imageToClassify, cv::COLOR_BGR2RGB);



    //cv::cvtColor(imageToClassify, imageToClassify, CV_BGR2RGB);

    frameTime2 = std::chrono::high_resolution_clock::now();

    classifyAndDisplayImage();

}

void ClassifyCameraStreamForm::on_btn_startClassify_clicked()
{
    if (ui->radioButton_classifyFromCamera->isChecked())
    {
        #ifndef TX1
            QMessageBox::question(this, "Unsuitable Hardware", "This function only works when the software has been compiled on the Jetson TX1 (or another device that has an appropriate device that outputs its feed in a way such that gstreamer can access it.",
                QMessageBox::Ok);
            return;
        #endif // TX1

        currentMode = FROM_CAMERA;

        ui->btn_pauseClassify->setEnabled(true);

        //we are on the TX1
        if (ui->radioButton_res2592x1458->isChecked())
            startClassifyFromCameraStream(nvCam2592x1458);
        else if (ui->radioButton_res1280x720->isChecked())
            startClassifyFromCameraStream(nvCam1280x720);

    }
    else if (ui->radioButton_classifyFromDisk->isChecked())
    {
        currentMode = FROM_DISK_IMAGES;

        ui->btn_pauseClassify->setEnabled(true);

        startClassifyFromDisk();
    }
}

void ClassifyCameraStreamForm::on_btn_selectTrainingData_clicked()
{
    QString toReadPath = QFileDialog::getOpenFileName(this, tr("Select the training data to load."),
                               "C:/",
                               tr("(*.xml)"));

    if (toReadPath == "")
        return;

    ic = ConverterMethods::getClassifierFromFile(toReadPath.toStdString());
}

void ClassifyCameraStreamForm::on_btn_selectImagesFromDisk_clicked()
{
    imagesFromDisk = QFileDialog::getOpenFileNames(this, tr("Select the training data to load."),
                               "C:/",
                               tr("Images(*.png *.bmp *.tiff *.jpg *.gif);;Any(*.*)"));


}

void ClassifyCameraStreamForm::on_btn_pauseClassify_clicked()
{
    timer->stop();
    ui->btn_continueClassify->setEnabled(true);
    determineBackAndForwardBtnStates();
    cap.release();
}

void ClassifyCameraStreamForm::on_btn_goBackOneImage_clicked()
{
    frameTime1 = std::chrono::high_resolution_clock::now();
    currentImageFromDisk--;
    imageToClassify = cv::imread(imagesFromDisk[currentImageFromDisk].toStdString());
    frameTime2 = std::chrono::high_resolution_clock::now();
    determineBackAndForwardBtnStates();
}

void ClassifyCameraStreamForm::on_btn_continueClassify_clicked()
{
    ui->btn_goBackOneImage->setEnabled(false);
    ui->btn_goForwardOneImage->setEnabled(false);
    timer->start();
}

void ClassifyCameraStreamForm::on_btn_goForwardOneImage_clicked()
{
    frameTime1 = std::chrono::high_resolution_clock::now();
    currentImageFromDisk++;
    imageToClassify = cv::imread(imagesFromDisk[currentImageFromDisk].toStdString());
    frameTime2 = std::chrono::high_resolution_clock::now();
    determineBackAndForwardBtnStates();
}

void ClassifyCameraStreamForm::determineBackAndForwardBtnStates()
{
    if (currentImageFromDisk > 0)
        ui->btn_goBackOneImage->setEnabled(true);
    if (currentImageFromDisk < imagesFromDisk.size() - 1)
        ui->btn_goForwardOneImage->setEnabled(true);
}

void ClassifyCameraStreamForm::on_btn_saveCurrentImageToDisk_clicked()
{
    QString savePathQ = QFileDialog::getSaveFileName(this, tr("Select the base name of the image."),
                               "XC:/",
                               tr("(*.png)"));

    if (savePathQ == "")
        return;

    std::string savePath = savePathQ.toStdString();

    savePath.erase(savePath.end() - 4, savePath.end()); //remove the extension

    std::string ext = ".bmp";

    std::string savePathOriginal = savePath + "-original_image" + ext;
    std::string savePathRecoloured = savePath + "-recoloured_image" + ext;
    std::string savePathPpBinary = savePath + "-pp_binary" + ext;
    std::string savePathPpRecoloured = savePath + "-pp_recoloured" + ext;

    cv::Mat img;



    if (ui->checkBox_saveOriginalImage->isChecked())
    {
        cv::cvtColor(this->imageToClassify, img, cv::COLOR_BGR2RGB);
        cv::imwrite(savePathOriginal, img);
    }


    if (ui->checkBox_saveRecolouredImage->isChecked())
    {
        cv::cvtColor(this->classImg.colouredImage, img, cv::COLOR_BGR2RGB);
        cv::imwrite(savePathRecoloured, img);
    }
    if (ui->checkBox_savePpBinary->isChecked())
    {
        cv::imwrite(savePathPpBinary, this->classImgPP.binaryImageMat);
    }

    if (ui->checkBox_savePpRecoloured->isChecked())
    {
        cv::cvtColor(this->classImgPP.colouredImage, img, cv::COLOR_BGR2RGB);
        cv::imwrite(savePathPpRecoloured, img);
    }
}

void ClassifyCameraStreamForm::setImageClassifier(std::shared_ptr<TextureClassifier> _ic)
{
    this->ic = _ic;
}

void ClassifyCameraStreamForm::on_btn_selectContinuousSaveLocation_clicked()
{
    QString savePath = QFileDialog::getSaveFileName(this, tr("Select the save location base file name."),
                               "",
                               tr("(*.*)"));

    continuousFileSavePath = savePath + "-original_image";
}
