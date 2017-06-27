#pragma once

#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <ImageSequence.h>
#include <tinyxml2.h>

//this class represents a generic texture classifier. that is, it decides whether or not a small rectangular image (a tile)
//is of a particular type. training data is supplied in the form of vectors of image sequences, to allow for pre clustering
//two vectors of image sequences may be supplied, one of which represents the target, and the other of which represents
//tiles that are not the target. for example, if the target tiles represent a road surface, the not target tiles would be
//tiles that often appear around the road surface to be classified.
//
//depending on the specific classification algorithm, only one of the supplied image sequences may be used. however, supplying
//both allows for the classification analysis code to determine the success rate in terms of both false positives and true positives
//after the fact.
class TextureClassifier
{
public:

    //typedef std::pair<std::string, ImageClassifier*> classifier;

    //we take in a vector of ImageSequences that might be pre-clustered
    //for example, when the image data is segmented by a person (manually), the person may indicate whether the
    //given image was taken in sunny or cloudy conditions. This data is therefore pre-clustered,
    //and should be passed as two separate image sequences.
    //the result is that the calculated thresholds for comparison (for CSA for example) will be higher,
    //and fewer false positives will occur.
    std::vector<ImageSequence> imageSequences_target;
    std::vector<ImageSequence> imageSequences_notTarget;

    static std::string getXmlAttributeName()
    {
        return "textureClassifierID";
    }

    TextureClassifier(){}

    TextureClassifier(
        std::vector<ImageSequence> _imageSequences_target,
        std::vector<ImageSequence> _imageSequences_notTarget);

    //train the model using the supplied data
	virtual void analyze() = 0;

    //classify a tile
    virtual bool isTarget(cv::Mat img) = 0;

    //TODO: make this non-virtual - getting image tile size should be generic
	virtual int getTileHeight() = 0;
	virtual int getTileWidth() = 0;

    virtual bool writeToFile(std::string filepath) = 0;
    virtual bool readFromFile(std::string filepath) = 0;

	class ClassifiedImage
	{
	public:
		//BinaryImage binaryImage;
		cv::Mat colouredImage;
		cv::Mat binaryImageMat;
		cv::Mat originalImage;

        ClassifiedImage(){}

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

    //if classifying a full image, this function may remove noise
    //performs erode -> dilate -> erode
    //the first erosion removes noise
    //the dilation closes holes (note that the dilation results in a larger binary image than the original, but with closed holes)
    //the second erosion return the regions of 1's in the binary image to their original size
	ClassifiedImage postProcessImage(ClassifiedImage classifiedImg);



	/*static ClassifiedImage classifyImage(
		vector<ImageClassifier> classifiers,
		Mat imgToClassify,
		int tileWidth,
		int tileHeight,
		bool genBinaryImg,
		bool genColouredImg);*/
};
