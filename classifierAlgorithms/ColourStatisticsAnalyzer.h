#pragma once

#include <TextureClassifier.h>
#include <ImageSequence.h>
#include <RgbHistogram.h>
#include <exception>

class ColourStatisticsAnalyzer : public TextureClassifier
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

	ColourStatisticsAnalyzer(
        vector<ImageSequence> _imageSequences_target,
        vector<ImageSequence> _imageSequences_notTarget,
		double _safetyFactor);

	vector<vector<RgbHistogram>> rgbHistograms;
	vector<RgbHistogram> averageRgbHistograms;

	virtual void analyze();
    virtual bool isTarget(Mat img);
	void writeHistogramData(string filepath);
	void writeAverageHistogramData(string filepath);

	virtual int getTileHeight();
	virtual int getTileWidth();

    virtual bool writeToFile(std::string filepath);
    virtual bool readFromFile(std::string filepath);

	void updateConstantImageSize();
};
