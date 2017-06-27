#include "TextureClassifier.h"

TextureClassifier::TextureClassifier(
    std::vector<ImageSequence> _imageSequences_target,
    std::vector<ImageSequence> _imageSequences_notTarget)
{
    imageSequences_target = _imageSequences_target;
    imageSequences_notTarget = _imageSequences_notTarget;
}

TextureClassifier::ClassifiedImage TextureClassifier::classifyImage(
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

            bool isTarget = false;

            isTarget = this->isTarget(subImg);

			if (genColouredImg)
			{
                if (isTarget)
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
                binImgMat.at<uchar>(bMapY, bMapX) = (isTarget) ? 255 : 0;

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

	

    TextureClassifier::ClassifiedImage cImg(img, binImgMat, imgToClassify);

	return cImg;
}

TextureClassifier::ClassifiedImage TextureClassifier::postProcessImage(TextureClassifier::ClassifiedImage classifiedImg)
{
	cv::Mat newColouredImg = classifiedImg.originalImage.clone();
	cv::Mat newBinaryImg = classifiedImg.binaryImageMat.clone();
	cv::Mat newOriginalImg = classifiedImg.originalImage.clone();

    TextureClassifier::ClassifiedImage cImg(newColouredImg, newBinaryImg, newOriginalImg);


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


static std::shared_ptr<TextureClassifier> getClassifierFromFile(std::string filepath)
{
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError eResult = doc.LoadFile(filepath.c_str());
    if (eResult != tinyxml2::XML_SUCCESS) throw std::runtime_error("Could not parse the supplied XML document. Check the formatting in your browser.");



    tinyxml2::XMLElement* titleElement = doc.FirstChildElement( "Texture_Classifier_Training_Data" );

    TextureClassifier toRet;

    const char* cID;
    cID = titleElement->Attribute("classifierID");

    std::string strID = std::string(cID);

    if (strID == ColourStatisticsAnalyzer::xmlID)
    {
        ColourStatisticsAnalyzer tmp;
        tmp.readFromFile(filepath);
        toRet = tmp;
    }

    return toRet;
}
