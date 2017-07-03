#pragma once

#include <TextureClassifier.h>
#include <ImageSequence.h>
#include <RgbHistogram.h>
#include <exception>
#include <tinyxml2.h>
#include <stdexcept>
#include <string>

class ColourStatisticsAnalyzerRgb : public TextureClassifier
{


	double safetyFactor;
	int tileWidth, tileHeight;

	vector<int> minThresR;
	vector<int> minThresG;
	vector<int> minThresB;


	void generateAverageRgbHistograms();
	void generateRgbHistogramsForImageSequences();
	void generateThresholds();


    vector<RgbHistogram> generateRgbHistogramsForImageSequence(ImageSequence targetImageSequence);

public:

    static std::string getXmlID();

    ColourStatisticsAnalyzerRgb();

    //note: internally preclustered data is treated the same way as non-preclustered data (just that
    //non-preclustered data has only 1 cluster - all of the training images).
	ColourStatisticsAnalyzerRgb(
        vector<ImageSequence> _imageSequences_target,
        vector<ImageSequence> _imageSequences_notTarget,
		double _safetyFactor);

    ColourStatisticsAnalyzerRgb(
            ImageSequence _imageSequence_target,
            ImageSequence _imageSequence_notTarget,
            double _safetyFactor);

	vector<vector<RgbHistogram>> rgbHistograms;
	vector<RgbHistogram> averageRgbHistograms;

	virtual void analyze();
    virtual bool isTarget(Mat img);
	void writeHistogramData(string filepath);
	void writeAverageHistogramData(string filepath);

	virtual int getTileHeight();
	virtual int getTileWidth();

    void setTileSize(int height, int width);


    virtual bool writeToFile(std::string filepath);
    virtual bool readFromFile(std::string filepath);

	void updateConstantImageSize();
};
