#pragma once
#include <ImageSequence.h>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

class ImageClassifier
{
public:
	std::vector<ImageSequence> imageSequences_road;
	std::vector<ImageSequence> imageSequences_notRoad;

	ImageClassifier(
		std::vector<ImageSequence> _imageSequences_road,
		std::vector<ImageSequence> _imageSequences_notRoad);

	virtual void analyze() = 0;

	virtual bool isRoad(cv::Mat img) = 0;

	virtual int getTileHeight() = 0;
	virtual int getTileWidth() = 0;

	class ClassifiedImage
	{
	public:
		//BinaryImage binaryImage;
		cv::Mat colouredImage;
		cv::Mat binaryImageMat;
		cv::Mat originalImage;

		ClassifiedImage(cv::Mat _colouredImage, cv::Mat _binaryImageMat, cv::Mat _originalImage)
		{
			colouredImage = _colouredImage;
			binaryImageMat = _binaryImageMat;
			originalImage = _originalImage;

		}
	};

	ClassifiedImage classifyImage(
		cv::Mat imgToClassify,
		bool genBinaryImg,
		bool genColouredImg);

	ClassifiedImage postProcessImage(ClassifiedImage classifiedImg);

	/*static ClassifiedImage classifyImage(
		vector<ImageClassifier> classifiers,
		Mat imgToClassify,
		int tileWidth,
		int tileHeight,
		bool genBinaryImg,
		bool genColouredImg);*/
};
