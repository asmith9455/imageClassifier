#include "ImageClassifier.h"

ImageClassifier::ImageClassifier(
	std::vector<ImageSequence> _imageSequences_road,
	std::vector<ImageSequence> _imageSequences_notRoad)
{
	imageSequences_road = _imageSequences_road;
	imageSequences_notRoad = _imageSequences_notRoad;	
}

ImageClassifier::ClassifiedImage ImageClassifier::classifyImage(
	cv::Mat imgToClassify,
	bool genBinaryImg,
	bool genColouredImg)
{

	cv::Mat img = imgToClassify.clone();


	int tileWidth = this->getTileWidth();
	int tileHeight = this->getTileHeight();

	int numShiftsX = img.cols / tileWidth;
	int numShiftsY = img.rows / tileHeight;

	cv::Mat binImgMat = cv::Mat(cv::Size(numShiftsX, numShiftsY), CV_8U);

	//BinaryImage binImg(numShiftsY, numShiftsX);

	int xPos = 0, yPos = 0;
	int bMapX = 0, bMapY = 0;

	while (true)
	{
		while (true)
		{

			cv::Mat subImg = img(cv::Range(yPos, yPos + tileHeight - 1), cv::Range(xPos, xPos + tileWidth - 1));

			//imshow("img", img);
			//waitKey(0);
			//destroyAllWindows();

			bool isRoad = false;

			isRoad = this->isRoad(subImg);

			if (genColouredImg)
			{
				if (isRoad)
				{
					subImg.forEach<cv::Point3_<uint8_t>>([&](cv::Point3_<uint8_t>& pixel, const int position[]) -> void {
						pixel.y = 255;	//set to green
					});
				}
				else
				{
					subImg.forEach<cv::Point3_<uint8_t>>([&](cv::Point3_<uint8_t>& pixel, const int position[]) -> void {
						pixel.z = 255;	//set to red
					});
				}
			}

			//binImg.setElement(bMapY, bMapX, isRoad);

			if (genBinaryImg)
				binImgMat.at<uchar>(bMapY, bMapX) = (isRoad) ? 255 : 0;

			bMapX++;

			xPos = xPos + tileWidth;
			if (xPos + tileWidth > img.cols)
				break;
		}
		bMapX = 0;
		bMapY++;
		xPos = 0;
		yPos = yPos + tileHeight;
		if (yPos + tileHeight > img.rows)
			break;
	}

	

	ImageClassifier::ClassifiedImage cImg(img, binImgMat, imgToClassify);

	return cImg;
}

ImageClassifier::ClassifiedImage ImageClassifier::postProcessImage(ImageClassifier::ClassifiedImage classifiedImg)
{
	cv::Mat newColouredImg = classifiedImg.originalImage.clone();
	cv::Mat newBinaryImg = classifiedImg.binaryImageMat.clone();
	cv::Mat newOriginalImg = classifiedImg.originalImage.clone();

	ImageClassifier::ClassifiedImage cImg(newColouredImg, newBinaryImg, newOriginalImg);


	//perform some post processing on the image to remove noise
	int type = cv::MORPH_ELLIPSE;
	int size1 = 1;
	int size2 = 4;
	int size3 = 3;

	cv::Mat element1 = cv::getStructuringElement(type,
		cv::Size(2 * size1 + 1, 2 * size1 + 1),
		cv::Point(size1, size1));

	cv::Mat element2 = cv::getStructuringElement(type,
		cv::Size(2 * size2 + 1, 2 * size2 + 1),
		cv::Point(size2, size2));

	cv::Mat element3 = cv::getStructuringElement(type,
		cv::Size(2 * size3 + 1, 2 * size3 + 1),
		cv::Point(size3, size3));

	cv::erode(cImg.binaryImageMat, cImg.binaryImageMat, element1);
	cv::dilate(cImg.binaryImageMat, cImg.binaryImageMat, element2);
	cv::erode(cImg.binaryImageMat, cImg.binaryImageMat, element3);



	int tileWidth = this->getTileWidth();
	int tileHeight = this->getTileHeight();

	int numShiftsX = cImg.colouredImage.cols / tileWidth;
	int numShiftsY = cImg.colouredImage.rows / tileHeight;

	//BinaryImage binImg(numShiftsY, numShiftsX);

	int xPos = 0, yPos = 0;
	int bMapX = 0, bMapY = 0;

	while (true)
	{
		while (true)
		{

			cv::Mat subImg = cImg.colouredImage(cv::Range(yPos, yPos + tileHeight - 1), cv::Range(xPos, xPos + tileWidth - 1));

			bool binImgTrue = cImg.binaryImageMat.at<uchar>(bMapY, bMapX) == 255;

			if (binImgTrue)
			{
				subImg.forEach<cv::Point3_<uint8_t>>([&](cv::Point3_<uint8_t>& pixel, const int position[]) -> void {
					pixel.y = 255;	//set to green
				});
			}
			else
			{
				subImg.forEach<cv::Point3_<uint8_t>>([&](cv::Point3_<uint8_t>& pixel, const int position[]) -> void {
					pixel.z = 255;	//set to red
				});
			}

			bMapX++;

			xPos = xPos + tileWidth;
			if (xPos + tileWidth > cImg.colouredImage.cols)
				break;
		}
		bMapX = 0;
		bMapY++;
		xPos = 0;
		yPos = yPos + tileHeight;
		if (yPos + tileHeight > cImg.colouredImage.rows)
			break;
	}


	return cImg;
}

//ImageClassifier::ClassifiedImage ImageClassifier::classifyImage(
//	vector<ImageClassifier> classifiers,
//	Mat imgToClassify,
//	int tileWidth,
//	int tileHeight,
//	bool genBinaryImg,
//	bool genColouredImg)
//{
//
//	Mat img = imgToClassify.clone();
//
//
//
//	int numShiftsX = img.cols / tileWidth;
//	int numShiftsY = img.rows / tileHeight;
//
//	Mat binImgMat = Mat(Size(numShiftsX, numShiftsY), CV_8U);
//
//	//BinaryImage binImg(numShiftsY, numShiftsX);
//
//	int xPos = 0, yPos = 0;
//
//	int bMapX = 0, bMapY = 0;
//
//
//
//	while (true)
//	{
//		while (true)
//		{
//
//			Mat subImg = img(cv::Range(yPos, yPos + tileHeight - 1), cv::Range(xPos, xPos + tileWidth - 1));
//
//			//imshow("img", img);
//			//waitKey(0);
//			//destroyAllWindows();
//
//			bool isRoad = false;
//			
//			for(int i = 0; i < classifiers.size(); i++)
//				isRoad = isRoad || classifiers[i].isRoad(subImg);
//
//			if (genColouredImg)
//			{
//				if (isRoad)
//				{
//					subImg.forEach<Point3_<uint8_t>>([&](Point3_<uint8_t>& pixel, const int position[]) -> void {
//						pixel.y = 255;	//set to green
//					});
//				}
//				else
//				{
//					subImg.forEach<Point3_<uint8_t>>([&](Point3_<uint8_t>& pixel, const int position[]) -> void {
//						pixel.z = 255;	//set to red
//					});
//				}
//			}
//
//			//binImg.setElement(bMapY, bMapX, isRoad);
//
//			binImgMat.at<uchar>(bMapY, bMapX) = (isRoad) ? 255 : 0;
//
//			bMapX++;
//
//			xPos = xPos + tileWidth;
//			if (xPos + tileWidth > img.cols)
//				break;
//		}
//		bMapX = 0;
//		bMapY++;
//		xPos = 0;
//		yPos = yPos + tileHeight;
//		if (yPos + tileHeight > img.rows)
//			break;
//	}
//
//	ImageClassifier::ClassifiedImage cImg(img, binImgMat);
//
//	return cImg;
//}