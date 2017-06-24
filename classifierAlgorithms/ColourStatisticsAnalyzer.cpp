#include <ColourStatisticsAnalyzer.h>

ColourStatisticsAnalyzer::ColourStatisticsAnalyzer(
    vector<ImageSequence> _imageSequences_target,
    vector<ImageSequence> _imageSequences_notTarget, double _safetyFactor) :
    TextureClassifier(_imageSequences_target, _imageSequences_notTarget)
{
	safetyFactor = _safetyFactor;
}

void ColourStatisticsAnalyzer::analyze()
{
	updateConstantImageSize();
	generateRgbHistogramsForImageSequences();
	generateAverageRgbHistograms();
	generateThresholds();
}

void ColourStatisticsAnalyzer::generateAverageRgbHistograms()
{
	averageRgbHistograms.clear();

	for (int i = 0; i < rgbHistograms.size(); i++)
	{
		vector<RgbHistogram> tmpVec2;
		tmpVec2 = RgbHistogram::averageRgbHistogramSequence(rgbHistograms[i]);

		for (int j = 0; j < tmpVec2.size(); j++)
			averageRgbHistograms.push_back(tmpVec2[j]);
	}
}

void ColourStatisticsAnalyzer::generateRgbHistogramsForImageSequences()
{
	rgbHistograms.clear();

    for (int i = 0; i < imageSequences_target.size(); i++)
	{
		vector<RgbHistogram> tmpHistos;

        tmpHistos = generateRgbHistogramsForImageSequence(imageSequences_target[i]);

		rgbHistograms.push_back(tmpHistos);
	}
}

void ColourStatisticsAnalyzer::generateThresholds()
{

	minThresR.clear();
	minThresG.clear();
	minThresB.clear();

	for (int i = 0; i < averageRgbHistograms.size(); i++)
	{
		int tmpMinThresR = INT_MAX;
		int tmpMinThresG = INT_MAX;
		int tmpMinThresB = INT_MAX;

		int sumThresR = 0;
		int sumThresG = 0;
		int sumThresB = 0;

		int tmp;
		for (int j = 0; j < rgbHistograms[i].size(); j++)
		{
			tmp = RgbHistogram::similarityR(averageRgbHistograms[i], rgbHistograms[i][j]);
			if (tmp < tmpMinThresR)
				tmpMinThresR = tmp;

			sumThresR += tmp;

			tmp = RgbHistogram::similarityG(averageRgbHistograms[i], rgbHistograms[i][j]);
			if (tmp < tmpMinThresG)
				tmpMinThresG = tmp;

			sumThresG += tmp;

			tmp = RgbHistogram::similarityB(averageRgbHistograms[i], rgbHistograms[i][j]);
			if (tmp < tmpMinThresB)
				tmpMinThresB = tmp;

			sumThresB += tmp;
		}

        // we can probably combine these double conversions with the calculations in the push back to avoid double alltogether
		double avgThresR = (double)sumThresR / (double)rgbHistograms[i].size();
		double avgThresG = (double)sumThresG / (double)rgbHistograms[i].size();
		double avgThresB = (double)sumThresB / (double)rgbHistograms[i].size();

		cout << rgbHistograms[i].size() << std::endl;
		cout << sumThresR << ", " << sumThresG << ", " << sumThresB << std::endl;
		cout << avgThresR << ", " << avgThresG << ", " << avgThresB << std::endl;

		minThresR.push_back(( avgThresR - (double)tmpMinThresR)*safetyFactor + (double)tmpMinThresR);
		minThresG.push_back((avgThresG - (double)tmpMinThresG)*safetyFactor + (double)tmpMinThresG);
		minThresB.push_back((avgThresB - (double)tmpMinThresB)*safetyFactor + (double)tmpMinThresB);

	}

}

//generate histograms for training data generated from a particular image
//(each image sequence contains training data from a particular image for automatic clustering purposes)
vector<RgbHistogram> ColourStatisticsAnalyzer::generateRgbHistogramsForImageSequence(ImageSequence targetImageSequence)
{
	//currently generates only a single histogram for each image sequence
	//(i.e. the vector returned will contain only 1 element).
	//plan to extend to k histos using a clustering algo

	vector<RgbHistogram> vecs;
    vecs = RgbHistogram::getHistograms(targetImageSequence);

	return vecs;

}

bool ColourStatisticsAnalyzer::isTarget(Mat img)
{
	RgbHistogram histo(img);
	int tempR, tempG, tempB;
	bool match = false;


    //iterate through the histograms that represent each pre-cluster of colour histogram data
	for (int i = 0; i < averageRgbHistograms.size(); i++)
	{
		tempR = RgbHistogram::similarityR(histo, averageRgbHistograms[i]);
		tempG = RgbHistogram::similarityG(histo, averageRgbHistograms[i]);
		tempB = RgbHistogram::similarityB(histo, averageRgbHistograms[i]);

		if (tempR >= minThresR[i] && tempG >= minThresG[i] && tempB >= minThresB[i])
		{
			match = true;
			break;
		}
	}

	return match;
}

void ColourStatisticsAnalyzer::writeHistogramData(string filepath)
{
	RgbHistogram::writeHistosToFile(rgbHistograms, filepath);
}

void ColourStatisticsAnalyzer::writeAverageHistogramData(string filepath)
{
	RgbHistogram::writeHistosToFile(averageRgbHistograms, filepath);
}

int ColourStatisticsAnalyzer::getTileHeight()
{
	return tileHeight;
}

int ColourStatisticsAnalyzer::getTileWidth()
{
	return tileWidth;
}

bool ColourStatisticsAnalyzer::writeToFile(std::string filepath)
{
    //we need to write all of the averageRgbHistograms and the threshold data for each channel for each of the histograms to file



    return false;
}

bool ColourStatisticsAnalyzer::readFromFile(std::string filepath)
{
    return false;
}

void ColourStatisticsAnalyzer::updateConstantImageSize()
{
	int constantSize = -1;
	bool constantBoolW = true, constantBoolH = true;
	int rows, cols;

    if (imageSequences_target.size() > 0 && imageSequences_target[0].getimageCount() > 0)
	{
        rows = imageSequences_target[0].imageAt(0).rows;
        cols = imageSequences_target[0].imageAt(0).cols;
	}

    for (int i = 0; i < imageSequences_target.size(); i++)
        for (int j = 0; j < imageSequences_target[i].getimageCount(); j++)
		{
            constantBoolH = constantBoolH && imageSequences_target[i].imageAt(j).rows == rows;
            constantBoolW = constantBoolW && imageSequences_target[i].imageAt(j).cols == cols;
		}

	if (constantBoolH)
		tileHeight = rows;

	if (constantBoolW)
		tileWidth = cols;
}
