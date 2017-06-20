#include "ClassifyCameraStreamForm.h"
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
    delete timer;
}


void ClassifyCameraStreamForm::startClassify()
{
#ifdef TX1
    std::cout << "Reading image database... ";
    vector<string> baseDir
        =
    {
        "/home/ubuntu/image_database/image_database_objects/32x32"
    };


    vector<ImageSequence> imgSequences_notRoad, imgSequences_road;

    int roadImageTotal = 0;
    int notRoadImageTotal = 0;
    int roadImageTotalForTraining = 0;

    for (int i = 0; i < baseDir.size(); i++)
    {
#ifdef Windows
        string notRoadDir = baseDir[i] + "\\notRoad";
        string roadDir = baseDir[i] + "\\road";
#endif

#ifdef Linux
        string notRoadDir = baseDir[i] + "/notRoad";
        string roadDir = baseDir[i] + "/road";
#endif

        ImageSequence roadSeq(roadDir);
        ImageSequence notRoadSeq(notRoadDir);

        roadImageTotal += roadSeq.getimageCount();
        notRoadImageTotal += notRoadSeq.getimageCount();

        imgSequences_notRoad.push_back(notRoadSeq);
        imgSequences_road.push_back(roadSeq);
    }

    vector<ImageSequence> imgSequences_road_reduced;

    double fractionUsedForTraining = 1.0;

    cout << "Using the following fraction of road images as training data: " << fractionUsedForTraining << std::endl;

    for (int i = 0; i < imgSequences_road.size(); i++)
    {
        int numImgs = (int)(fractionUsedForTraining * (double)imgSequences_road[i].getimageCount());
        ImageSequence imgSequence_road_reduced = ImageSequence::getRandomImageSequence(imgSequences_road[i], numImgs);
        imgSequences_road_reduced.push_back(imgSequence_road_reduced);
        roadImageTotalForTraining += imgSequence_road_reduced.getimageCount();
    }


    //imshow("orig", imgSequences_road[0].imageAt(0));
    //imshow("new", imgSequences_road_reduced[0].imageAt(0));

    //waitKey(0);                                          // Wait for a keystroke in the window
    //destroyAllWindows();



    cout << "Done." << std::endl;

    cout << "Read a total of " << roadImageTotal << " road images." << std::endl;
    cout << "Read a total of " << notRoadImageTotal << " notRoad images." << std::endl;

    cout << "Selected a total of " << roadImageTotalForTraining << " road images for training." << std::endl;

    if (roadImageTotal == 0)
        throw runtime_error("Training Data not found.");


    cout << "Initializing ColourStatisticsAnalyzer... ";
    csa = new ColourStatisticsAnalyzer(imgSequences_road_reduced, imgSequences_notRoad, 0.1);


    //cout << "csa 1 const image size: " << csa1.constImageSize() << std::endl;
    //ic = new (ImageClassifier)(csa);

    cout << "Training model (1)... ";
    csa->analyze();
    cout << "Done." << std::endl;

    ic = static_cast<ImageClassifier*>(csa);

    string name;

    cout << "Starting image classification loop... ";

    string imageToTest = "/home/ubuntu/image_database/image_database_parkingLot/galaxys7/test_img_0000.jpg";

    //string imageToTest = "C:\\Users\\Joe\\OneDrive\\Capstone 2017\\image_database_parkingLot\\galaxys7\\test_img_0000.jpg";



    //const char* gst =  "nvcamerasrc ! video/x-raw(memory:NVMM), width=(int)1280, height=(int)720, format=(string)I420, framerate=(fraction)120/1 ! nvvidconv flip-method=2 ! video/x-raw, format=(string)I420 ! videoconvert ! video/x-raw, format=(string)BGR ! appsink";
    //const char* gst = "nvcamerasrc ! video/x-raw(memory:NVMM), width=(int)2592, height=(int)1458, format=(string)I420, framerate=(fraction)30/1 ! nvvidconv flip-method=2 ! video/x-raw, format=(string)I420 ! videoconvert ! video/x-raw, format=(string)BGR ! appsink";
    cap = VideoCapture(gst);
    //cap.set(CV_CAP_PROP_BUFFERSIZE, 3);
    //cap.set(CV_CAP_PROP_FPS, 30);
    /*if (!cap.isOpened()) { // check if we succeeded
        throw runtime_error("Could not open camera.");
    }*/


    timer = new QTimer(this);

    connect(timer, SIGNAL(timeout()), this, SLOT(displayVideo()));
    timer->start(20);

    return;

#endif
}

void ClassifyCameraStreamForm::displayVideo()
{
    cap >> img; // get a new frame from camera
    ImageClassifier::ClassifiedImage classImg = ic->classifyImage(img, true, true);
    ImageClassifier::ClassifiedImage classImgPP = ic->postProcessImage(classImg);

    cv::cvtColor(img, img, CV_BGR2RGB);
    cv::cvtColor(classImg.colouredImage, classImg.colouredImage, CV_BGR2RGB);
    cv::cvtColor(classImgPP.colouredImage, classImgPP.colouredImage, CV_BGR2RGB);
    //cv::cvtColor(classImg.binaryImageMat, classImg.binaryImageMat, CV_BGR2RGB);
    //cv::imshow()
    ui->label_originalImage->setPixmap(QPixmap::fromImage(QImage(img.data, img.cols, img.rows, img.step, QImage::Format_RGB888)));
    ui->label_classifiedImagePostProcessed->setPixmap(QPixmap::fromImage(QImage(classImgPP.colouredImage.data, classImgPP.colouredImage.cols, classImgPP.colouredImage.rows, classImgPP.colouredImage.step, QImage::Format_RGB888)));
    ui->label_classifiedImageBinary->setPixmap(QPixmap::fromImage(QImage(classImg.colouredImage.data, classImg.colouredImage.cols, classImg.colouredImage.rows, classImg.colouredImage.step, QImage::Format_RGB888)));
}
