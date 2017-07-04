#pragma once

#include <TextureClassifier.h>
#include <ImageSequence.h>
#include <RgbHistogram.h>
#include <exception>
#include <tinyxml2.h>
#include <stdexcept>
#include <string>
#include <numeric>
#include <limits>

//adaptation of ColourStatisticsAnalyzerRgb - also uses some information about colour change magnitude and uniformity
//does not implement pre-clustering - assumed minimal benefit based on past experience
class CSA_RGB_shiftHisto : public TextureClassifier
{

	double safetyFactor;
	int tileWidth, tileHeight;

    int maxThresR;
    int maxThresG;
    int maxThresB;

    float rowAvgChange;
    float rowStdDevChange;

    float minRatio, maxRatio;

    std::vector<RgbHistogram> rgbHistograms;
    RgbHistogram averageRgbHistogram;

    void generateAverageRgbHistogram();
    void generateRgbHistogramsForImageSequence();
    void generateHistoThresholds();
    void generateChangeStats();
    void generateChangeThresholds();
    void rotateHistogramMeans();

    vector<RgbHistogram> generateRgbHistogramsForImageSequence(ImageSequence targetImageSequence);

public:

    static std::string getXmlID();

    CSA_RGB_shiftHisto();

    //note: internally preclustered data is treated the same way as non-preclustered data (just that
    //non-preclustered data has only 1 cluster - all of the training images).

    CSA_RGB_shiftHisto(
            ImageSequence _imageSequence_target,
            ImageSequence _imageSequence_notTarget,
            double _safetyFactor);



	virtual void analyze();
    virtual bool isTarget(Mat img);

	virtual int getTileHeight();
	virtual int getTileWidth();

    void setTileSize(int height, int width);


    virtual bool writeToFile(std::string filepath);
    virtual bool readFromFile(std::string filepath);

	void updateConstantImageSize();
};
