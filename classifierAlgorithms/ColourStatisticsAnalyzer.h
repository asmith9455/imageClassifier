#pragma once

#include "TextureClassifier.h"
#include "ImageSequence.h"
#include "RgbHistogram.h"
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


	vector<RgbHistogram> generateRgbHistogramsForImageSequence(ImageSequence roadImageSequence);

public:

	ColourStatisticsAnalyzer(
		vector<ImageSequence> _imageSequences_road,
        vector<ImageSequence> _imageSequences_notRoad,
		double _safetyFactor);

	vector<vector<RgbHistogram>> rgbHistograms;
	vector<RgbHistogram> averageRgbHistograms;

	virtual void analyze();
	virtual bool isRoad(Mat img);
	void writeHistogramData(string filepath);
	void writeAverageHistogramData(string filepath);

	virtual int getTileHeight();
	virtual int getTileWidth();

	void updateConstantImageSize();
};
