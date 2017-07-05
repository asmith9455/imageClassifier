#pragma once

#include <TextureClassifier.h>
#include <ImageSequence.h>
#include <exception>
#include <tinyxml2.h>
#include <stdexcept>
#include <opencv2/core/core.hpp>
#include <opencv2/ml/ml.hpp>
#include <ConverterMethods.h>

//adaptation of ColourStatisticsAnalyzerRgb - also uses some information about colour change magnitude and uniformity
//does not implement pre-clustering - assumed minimal benefit based on past experience
class IMG_SVM : public TextureClassifier
{
private:
    typedef cv::ml::SVM cvSVM;

	int tileWidth, tileHeight;

    cv::Ptr<cv::ml::SVM> svm;

public:

    static std::string getXmlID();

    IMG_SVM();

    //note: internally preclustered data is treated the same way as non-preclustered data (just that
    //non-preclustered data has only 1 cluster - all of the training images).

    IMG_SVM(
            ImageSequence _imageSequence_target,
            ImageSequence _imageSequence_notTarget);



	virtual void analyze();
    virtual bool isTarget(cv::Mat img);

	virtual int getTileHeight();
	virtual int getTileWidth();




    virtual bool writeToFile(std::string filepath);
    virtual bool readFromFile(std::string filepath);

    void setTileSize(int height, int width);
	void updateConstantImageSize();
};
