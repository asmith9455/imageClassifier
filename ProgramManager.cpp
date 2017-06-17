#include <ProgramManager.h>

/*void ProgramManager::classifyDisplaySingleImage(std::string imagePath)
{

}*/

void ProgramManager::trainClassifyDisplaySingleImage()
{
	std::cout << "Reading image database... ";


#ifdef Windows
	vector<string> baseDir
		=
	{
		//"C:\\Users\\Alex\\OneDrive\\Capstone 2017\\image_database_objects\\640x480\\bb2\\12x12, summer, clear, east"
		//"C:\\Users\\Alex\\OneDrive\\Capstone 2017\\image_database_objects\\640x480\\s7(downscaled)\\16x16"
		//"C:\\Users\\Alex\\OneDrive\\Capstone 2017\\image_database_objects\\4032x3024\\32x32"
		//"C:\\Users\\Alex\\OneDrive\\Capstone 2017\\image_database_objects\\2592x1458\\8x8_noshadow"
		//"C:\\Users\\Alex\\OneDrive\\Capstone 2017\\image_database_objects\\2592x1458\\16x16_stopMarkerLit"
		"C:\\Users\\Alex\\OneDrive\\Capstone 2017\\image_database_objects\\2592x1458\\8x8_stopMarkerLit"
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

		roadImageTotal += roadSeq.getimageCount();
		notRoadImageTotal += notRoadSeq.getimageCount();

		imgSequences_notRoad.push_back(notRoadSeq);
		imgSequences_road.push_back(roadSeq);
	}

	vector<ImageSequence> imgSequences_road_reduced;

	double fractionUsedForTraining = 0.05;

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

	std::cout << "Read a total of " << roadImageTotal << " road images." << std::endl;
	std::cout << "Read a total of " << notRoadImageTotal << " notRoad images." << std::endl;

	std::cout << "Selected a total of " << roadImageTotalForTraining << " road images for training." << std::endl;

	if (roadImageTotal == 0)
		throw runtime_error("Training Data not found.");


	std::cout << "Initializing ColourStatisticsAnalyzer... ";
	ColourStatisticsAnalyzer csa1(imgSequences_road_reduced, imgSequences_notRoad, 0.01);
	std::cout << "Done." << std::endl;

	ImageClassifier* ic = &csa1;

	std::cout << "csa 1 const image size: " << ic->getTileHeight() << " rows x " << ic->getTileWidth() << " cols" << std::endl;


	std::cout << "Training model... ";
	csa1.analyze();
	std::cout << "Done." << std::endl;


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
		std::cout << "Done. Output: " << std::endl;
		std::cout << "----True Positives: " << 100.0 * (double)trueMatches / (double)trueCount << "%" << std::endl;
		std::cout << "----False Positives: " << 100.0 * (double)falseMatches / (double)falseCount << "%" << std::endl;
	}

	if (classifyIndividualImage)
	{

		string name;

		std::cout << "Classifying individual image... ";
		std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();


#ifdef Windows
		//string imageToTest = "C:\\Users\\Alex\\OneDrive\\Capstone 2017\\image_database_parkingLot\\bb2\\2017-05-15, East only\\testVideo1494238444_00000000.tiff";
		//string imageToTest = "C:\\Users\\Alex\\OneDrive\\Capstone 2017\\image_database_parkingLot\\galaxys7\\test_img_0002.jpg";
		//string imageToTest = "C:\\Users\\Alex\\OneDrive\\Capstone 2017\\image_database_parkingLot\\galaxys7\\test_img_0001.png";
		//string imageToTest = "C:\\Users\\Alex\\OneDrive\\Capstone 2017\\image_database_parkingLot\\galaxys7\\test_img_0000.jpg";
		//string imageToTest = "C:\\Users\\Alex\\OneDrive\\Capstone 2017\\image_database_parkingLot\\ov5693\\orig559.png";		//has cei and road in it
		//string imageToTest = "C:\\Users\\Alex\\OneDrive\\Capstone 2017\\image_database_parkingLot\\ov5693\\orig1630.png";		//has stop Marker in it (lit) (very close)
		string imageToTest = "C:\\Users\\Alex\\OneDrive\\Capstone 2017\\image_database_parkingLot\\ov5693\\orig1621.png";		//has stop Marker in it (lit)
		//string imageToTest = "C:\\Users\\Alex\\OneDrive\\Capstone 2017\\image_database_parkingLot\\ov5693\\orig1594.png";		//has stop Marker in it (lit)
		//string imageToTest = "C:\\Users\\Alex\\OneDrive\\Capstone 2017\\image_database_parkingLot\\ov5693\\orig1549.png";		//has stop Marker in it (lit) (very distant)

		//string imageToTest = "C:\\Users\\Alex\\OneDrive\\Capstone 2017\\image_database_parkingLot\\ov5693\\orig1081.png";		//has stop Marker in it (unlit)
#endif

#ifdef Linux
		string imageToTest = "/home/ubuntu/image_database/image_database_parkingLot/galaxys7/test_img_0000.jpg";
#endif

		//string imageToTest = "C:\\Users\\Joe\\OneDrive\\Capstone 2017\\image_database_parkingLot\\galaxys7\\test_img_0000.jpg";

		cv::Mat img = imread(imageToTest, CV_LOAD_IMAGE_COLOR);

		std::cout << "Read image to test of dimension " << img.cols << " by " << img.rows << std::endl;

		ImageClassifier::ClassifiedImage classImg = ic->classifyImage(img, true, true);

		ImageClassifier::ClassifiedImage classImgPP = ic->postProcessImage(classImg);

		//GaussianBlur(classImg.binaryImageMat, classImg.binaryImageMat, Size(3, 3), 0, 0);

		std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
		double dif = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

		std::cout << "Done. ( Classified in " << dif << " ms )" << std::endl;


		cv::Mat resizeBin, resizeImg, resizeBinPP, resizeImgPP;
		//resize(classImg.binaryImageMat, resizeBin, Size(640,960), 0, 0, INTER_NEAREST);


		//resize images to be displayed
		cv::resize(classImg.binaryImageMat, resizeBin, cv::Size(960, 540), 0, 0, INTER_LINEAR);
		cv::resize(classImg.colouredImage, resizeImg, cv::Size(960, 540), 0, 0, INTER_LINEAR);
		cv::resize(classImgPP.binaryImageMat, resizeBinPP, cv::Size(960, 540), 0, 0, INTER_LINEAR);
		cv::resize(classImgPP.colouredImage, resizeImgPP, cv::Size(960, 540), 0, 0, INTER_LINEAR);

		//namedWindow("Display window", WINDOW_AUTOSIZE);		// Create a window for display.

		//cv::imwrite("testWrite.png", classImg.binaryImageMat);
		cv::imshow("binary image", resizeBin);
		cv::imshow("coloured image", resizeImg);						// Show our image inside it.

		cv::imshow("binary image pp", resizeBinPP);
		cv::imshow("coloured image pp", resizeImgPP);						// Show our image inside it.

		waitKey(0);                                          // Wait for a keystroke in the window
		cv::destroyAllWindows();
	}

}

void ProgramManager::trainClassifyDisplayCameraStream()
{
	std::cout << "Running test 0003. This test classifies images captured by the TX1's onboard camera. The classified images are displayed using opencv." << std::endl;


#ifndef TX1
	throw runtime_error("This function has only been tested on the NVIDIA Jetson TX1. It will only run if this application has been built with TX1 defined in applicationDefines.h.");
#endif // TX1

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
	ColourStatisticsAnalyzer csa1(imgSequences_road_reduced, imgSequences_notRoad, 0.1);


	//cout << "csa 1 const image size: " << csa1.constImageSize() << std::endl;
	ImageClassifier* ic = &csa1;

	cout << "Training model (1)... ";
	csa1.analyze();
	cout << "Done." << std::endl;

	string name;

	cout << "Starting image classification loop... ";

	string imageToTest = "/home/ubuntu/image_database/image_database_parkingLot/galaxys7/test_img_0000.jpg";

	//string imageToTest = "C:\\Users\\Joe\\OneDrive\\Capstone 2017\\image_database_parkingLot\\galaxys7\\test_img_0000.jpg";



	//const char* gst =  "nvcamerasrc ! video/x-raw(memory:NVMM), width=(int)1280, height=(int)720, format=(string)I420, framerate=(fraction)120/1 ! nvvidconv flip-method=2 ! video/x-raw, format=(string)I420 ! videoconvert ! video/x-raw, format=(string)BGR ! appsink";
	const char* gst = "nvcamerasrc ! video/x-raw(memory:NVMM), width=(int)2592, height=(int)1458, format=(string)I420, framerate=(fraction)30/1 ! nvvidconv flip-method=2 ! video/x-raw, format=(string)I420 ! videoconvert ! video/x-raw, format=(string)BGR ! appsink";
	VideoCapture cap(gst);

	if (!cap.isOpened()) { // check if we succeeded
		throw runtime_error("Could not open camera.");
	}

	Mat resizeBin, resizeImg;
	namedWindow("Classified Image: Colour", WINDOW_AUTOSIZE);		// Create a window for display.
	namedWindow("Classified Image: Binary", WINDOW_AUTOSIZE);		// Create a window for display.
	Mat img;
	double dif;
	double totalTime = 0;

	cout << std::endl;
	int imagesClassified = 0;
	int frameDiv = 0;

	while ((char)27 != (char)waitKey(1))
	{
		imagesClassified++;
		std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

		cap >> img; // get a new frame from camera

		ImageClassifier::ClassifiedImage classImg = ic->classifyImage(img, true, true);
		ImageClassifier::ClassifiedImage classImgPP = ic->postProcessImage(classImg);


		resize(classImg.binaryImageMat, resizeBin, Size(960, 540), 0, 0, INTER_LINEAR);
		resize(classImg.colouredImage, resizeImg, Size(960, 540), 0, 0, INTER_LINEAR);

		if (frameDiv == 9)
		{
            string path = "test1/";
            string imgName ="orig";
            string num = to_string(imagesClassified);
            string ext = ".png";
            string origImgName = path + imgName + num + ext;
			frameDiv = 0;
			cv::imwrite(origImgName,img);
			//cv::imwrite("coloured"+imagesClassified + ".png",classImg.colouredImage);
			//cv::imwrite("binaryImg"+imagesClassified+ ".png", classImg.binaryImageMat);
		}

		imshow("Classified Image: Colour", resizeImg);						// Show our image inside it.
		imshow("Classified Image: Binary", resizeBin);

		std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
		dif = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
		totalTime += dif;
		//cout << string(str.length(), '\b');
		//string s =
		//cout << "Processed frame in " << dif << " ms.";

		frameDiv++;

	}

	cout << "Averge time to process 1 frame: " << totalTime / (double)imagesClassified << " ms.";

	cout << std::endl;
	cap.release();
	// Wait for a keystroke in the window
	destroyAllWindows();

#endif
}
