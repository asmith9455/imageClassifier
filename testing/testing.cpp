#include "testing.h"


int test0001()
{

	std::cout << "Reading image database... ";


	#ifdef Windows
	vector<string> baseDir
		=
	{
		//"C:\\Users\\Alex\\OneDrive\\Capstone 2017\\image_database_objects\\640x480\\bb2\\12x12, summer, clear, east"
		//"C:\\Users\\Alex\\OneDrive\\Capstone 2017\\image_database_objects\\640x480\\s7(downscaled)\\16x16"
		"C:\\Users\\Alex\\OneDrive\\Capstone 2017\\image_database_objects\\4032x3024\\32x32"
		//"C:\\Users\\Joe\\OneDrive\\Capstone 2017\\image_database_objects\\4032x3024\\32x32"
	};
	#endif

	#ifdef Linux
	vector<string> baseDir
		=
	{
		"/home/ubuntu/image_database/image_database_objects/32x32"
	};
	#endif
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

		roadImageTotal+=roadSeq.getimageCount();
		notRoadImageTotal+=notRoadSeq.getimageCount();

		imgSequences_notRoad.push_back(notRoadSeq);
		imgSequences_road.push_back(roadSeq);
	}

	vector<ImageSequence> imgSequences_road_reduced;

	double fractionUsedForTraining = 1.0;

    std::cout << "Using the following fraction of road images as training data: " << fractionUsedForTraining << std::endl;

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



	std::cout << "Done." << std::endl;

	std::cout<<"Read a total of "<<roadImageTotal<<" road images."<<std::endl;
	std::cout<<"Read a total of "<<notRoadImageTotal<<" notRoad images."<<std::endl;

	std::cout<<"Selected a total of "<<roadImageTotalForTraining<<" road images for training."<<std::endl;

	if (roadImageTotal == 0)
		throw runtime_error("Training Data not found.");


	std::cout << "Initializing ColourStatisticsAnalyzer... ";
	ColourStatisticsAnalyzer csa1(imgSequences_road_reduced, imgSequences_notRoad, 0.1);


	ColourStatisticsAnalyzer csa2(imgSequences_road_reduced, imgSequences_notRoad, 0.1);
	std::cout << "Done." << std::endl;

	bool imgIsEqual = ImageSequence::imagesAreEqual(csa1.imageSequences_road[0].imageAt(0), csa2.imageSequences_road[0].imageAt(0));
	bool imgsEqual = ImageSequence::imagesSequencesAreEqual(csa1.imageSequences_road[0], csa2.imageSequences_road[0]);
    std::cout << "Images are equal: " << imgsEqual << std::endl;


	std::cout << "csa 1 const image size: " << csa1.getTileHeight() << " rows x " << csa1.getTileWidth() << " cols" << std::endl;
	std::cout << "csa 2 const image size: " << csa2.getTileHeight() << " rows x " << csa2.getTileWidth() << " cols" << std::endl;

	ImageClassifier* ic = &csa1;

	std::cout << "Training model (1)... ";
	csa1.analyze();
	std::cout << "Done." << std::endl;

	std::cout << "Training model (2)... ";
	csa2.analyze();
	std::cout << "Done." << std::endl;

	bool histoEqual = RgbHistogram::AreEqual(csa1.rgbHistograms[0][0], csa2.rgbHistograms[0][0]);
	bool histosEqual = RgbHistogram::ListOfListIsEqual(csa1.rgbHistograms, csa2.rgbHistograms);
	std::cout << "Histograms are equal: " << histosEqual << std::endl;

	bool measureStatisticsUsingImageDatabase = true;
	bool classifyIndividualImage = true;


	if (measureStatisticsUsingImageDatabase)
	{
		std::cout << "Measuring statistics using image database... ";
		int trueMatches = 0;
		int trueCount = 0;
		int falseMatches = 0;
		int falseCount = 0;

		for (int i = 0; i < imgSequences_road.size(); i++)
		{
			for (int j = 0; j < imgSequences_road[i].getimageCount(); j++)
			{
				if (ic->isRoad(imgSequences_road[i].imageAt(j)))
					trueMatches++;
				trueCount++;
			}
		}

		for (int i = 0; i < imgSequences_notRoad.size(); i++)
		{
			for (int j = 0; j < imgSequences_notRoad[i].getimageCount(); j++)
			{
				if (ic->isRoad(imgSequences_notRoad[i].imageAt(j)))
					falseMatches++;
				falseCount++;
			}
		}
		std::cout << "Done. Output: "<<std::endl;
		std::cout << "----True Positives: " << 100.0 * (double)trueMatches / (double)trueCount << "%" << std::endl;
		std::cout << "----False Positives: " << 100.0 * (double)falseMatches / (double)falseCount << "%" << std::endl;
	}




	if (classifyIndividualImage)
	{

		string name;

		std::cout << "Classifying individual image... ";
		high_resolution_clock::time_point t1 = high_resolution_clock::now();


		#ifdef Windows
		//string imageToTest = "C:\\Users\\Alex\\OneDrive\\Capstone 2017\\image_database_parkingLot\\bb2\\2017-05-15, East only\\testVideo1494238444_00000000.tiff";
		//string imageToTest = "C:\\Users\\Alex\\OneDrive\\Capstone 2017\\image_database_parkingLot\\galaxys7\\test_img_0002.jpg";
		//string imageToTest = "C:\\Users\\Alex\\OneDrive\\Capstone 2017\\image_database_parkingLot\\galaxys7\\test_img_0001.png";
		string imageToTest = "C:\\Users\\Alex\\OneDrive\\Capstone 2017\\image_database_parkingLot\\galaxys7\\test_img_0000.jpg";
		#endif

		#ifdef Linux
		string imageToTest = "/home/ubuntu/image_database/image_database_parkingLot/galaxys7/test_img_0000.jpg";
		#endif

		//string imageToTest = "C:\\Users\\Joe\\OneDrive\\Capstone 2017\\image_database_parkingLot\\galaxys7\\test_img_0000.jpg";

		Mat img = imread(imageToTest, CV_LOAD_IMAGE_COLOR);

		std::cout << "Read image to test of dimension " << img.cols << " by " << img.rows << std::endl;

		ImageClassifier::ClassifiedImage classImg = ic->classifyImage(img, true, true);

		//GaussianBlur(classImg.binaryImageMat, classImg.binaryImageMat, Size(3, 3), 0, 0);

		high_resolution_clock::time_point t2 = high_resolution_clock::now();
		double dif = duration_cast<milliseconds>(t2 - t1).count();

		std::cout << "Done. ( " << dif << " ms )" <<  std::endl;


		Mat resizeBin, resizeImg;
		//resize(classImg.binaryImageMat, resizeBin, Size(640,960), 0, 0, INTER_NEAREST);


		//resize images to be displayed
		resize(classImg.binaryImageMat, resizeBin, Size(960, 540), 0, 0, INTER_LINEAR);
		resize(classImg.colouredImage, resizeImg, Size(960, 540), 0, 0, INTER_LINEAR);

		imwrite("testWrite.png", classImg.binaryImageMat);

		imshow("testBin", resizeBin);

		namedWindow("Display window", WINDOW_AUTOSIZE);		// Create a window for display.
		imshow("Display window", resizeImg);						// Show our image inside it.

		waitKey(0);                                          // Wait for a keystroke in the window
		destroyAllWindows();

		//csa1.writeHistogramData("C:\\Users\\Alex\\OneDrive\\Capstone 2017\\histogram_data\\histoData2.txt");
		//csa1.writeAverageHistogramData("C:\\Users\\Alex\\OneDrive\\Capstone 2017\\histogram_data\\avgHistoData2.txt");

		//cout << csa1.isRoad(Mat()) << std::endl;

	}

	return 0;
}

int test0002()
{
	//test opencv


	Mat image;
	image = imread("C:\\Users\\Alex\\OneDrive\\Capstone 2017\\code_alex\\road_classifier\\x64\\Debug\\testImage.jpg", CV_LOAD_IMAGE_COLOR);   // Read the file

	if (!image.data)                              // Check for invalid input
	{
		throw runtime_error("Could not open the image.");
	}

	namedWindow("Display window", WINDOW_AUTOSIZE);// Create a window for display.
	imshow("Display window", image);                   // Show our image inside it.

	waitKey(0);                                          // Wait for a keystroke in the window

	destroyAllWindows();

	//system("pause");

	return 0;
}

int test0003()
{
    std::cout << "Running test 0003. This test classifies images captured by the TX1's onboard camera. The classified images are displayed using opencv." << std::endl;
	std::cout << "Reading image database... ";

    #ifndef TX1
    throw runtime_error("This function has only been tested on the NVIDIA Jetson TX1. It will only run if this application has been built with TX1 defined in applicationDefines.h.");
    #endif // TX1

	#ifdef TX1
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

		roadImageTotal+=roadSeq.getimageCount();
		notRoadImageTotal+=notRoadSeq.getimageCount();

		imgSequences_notRoad.push_back(notRoadSeq);
		imgSequences_road.push_back(roadSeq);
	}

	vector<ImageSequence> imgSequences_road_reduced;

	double fractionUsedForTraining = 1.0;

    std::cout << "Using the following fraction of road images as training data: " << fractionUsedForTraining << std::endl;

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



	std::cout << "Done." << std::endl;

	std::cout<<"Read a total of "<<roadImageTotal<<" road images."<<std::endl;
	std::cout<<"Read a total of "<<notRoadImageTotal<<" notRoad images."<<std::endl;

	std::cout<<"Selected a total of "<<roadImageTotalForTraining<<" road images for training."<<std::endl;

	if (roadImageTotal == 0)
		throw runtime_error("Training Data not found.");


	std::cout << "Initializing ColourStatisticsAnalyzer... ";
	ColourStatisticsAnalyzer csa1(imgSequences_road_reduced, imgSequences_notRoad, 0.1);


	//std::cout << "csa 1 const image size: " << csa1.constImageSize() << std::endl;
	ImageClassifier* ic = &csa1;

	std::cout << "Training model (1)... ";
	csa1.analyze();
	std::cout << "Done." << std::endl;

    string name;

    std::cout << "Starting image classification loop... ";

    string imageToTest = "/home/ubuntu/image_database/image_database_parkingLot/galaxys7/test_img_0000.jpg";

    //string imageToTest = "C:\\Users\\Joe\\OneDrive\\Capstone 2017\\image_database_parkingLot\\galaxys7\\test_img_0000.jpg";



    //const char* gst =  "nvcamerasrc ! video/x-raw(memory:NVMM), width=(int)1280, height=(int)720, format=(string)I420, framerate=(fraction)120/1 ! nvvidconv flip-method=2 ! video/x-raw, format=(string)I420 ! videoconvert ! video/x-raw, format=(string)BGR ! appsink";
    const char* gst =  "nvcamerasrc ! video/x-raw(memory:NVMM), width=(int)2592, height=(int)1458, format=(string)I420, framerate=(fraction)30/1 ! nvvidconv flip-method=2 ! video/x-raw, format=(string)I420 ! videoconvert ! video/x-raw, format=(string)BGR ! appsink";
    VideoCapture cap(gst);

    if(!cap.isOpened()) { // check if we succeeded
		throw runtime_error("Could not open camera.");
	}

	Mat resizeBin, resizeImg;
    namedWindow("Classified Image: Colour", WINDOW_AUTOSIZE);		// Create a window for display.
    namedWindow("Classified Image: Binary", WINDOW_AUTOSIZE);		// Create a window for display.
    Mat img;
    double dif;
    double totalTime = 0;

    std::cout<<std::endl;
    int imagesClassified = 0;
    while((char)27 != (char) waitKey(1))
    {
        imagesClassified++;
        high_resolution_clock::time_point t1 = high_resolution_clock::now();

		cap >> img; // get a new frame from camera

        ImageClassifier::ClassifiedImage classImg = ic->classifyImage(img, true, true);
        resize(classImg.binaryImageMat, resizeBin, Size(960, 540), 0, 0, INTER_LINEAR);
        resize(classImg.colouredImage, resizeImg, Size(960, 540), 0, 0, INTER_LINEAR);


        imshow("Classified Image: Colour", resizeImg);						// Show our image inside it.
        imshow("Classified Image: Binary", resizeBin);

        high_resolution_clock::time_point t2 = high_resolution_clock::now();
        dif = duration_cast<milliseconds>(t2 - t1).count();
        totalTime+=dif;
        //std::cout << string(str.length(), '\b');
        //string s =
        //std::cout << "Processed frame in " << dif << " ms.";

    }

    std::cout << "Averge time to process 1 frame: " << totalTime/(double)imagesClassified << " ms.";

    std::cout << std::endl;
    cap.release();
                                          // Wait for a keystroke in the window
    destroyAllWindows();

	return 0;

	#endif
}
