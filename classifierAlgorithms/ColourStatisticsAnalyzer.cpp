#include <ColourStatisticsAnalyzer.h>

ColourStatisticsAnalyzer::ColourStatisticsAnalyzer(){}

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

        // TODO: we can probably combine these double conversions with the calculations in the push back to avoid double completely
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
    //using namespace tinyxml2;

    tinyxml2::XMLDocument doc;

    tinyxml2::XMLDeclaration * decl = doc.NewDeclaration("xml version=\"1.0\" encoding=\"UTF-8\"");
    doc.LinkEndChild( decl );

    tinyxml2::XMLElement *node1 = doc.NewElement("Texture_Classifier_Training_Data");

    node1->SetAttribute("classifier", ColourStatisticsAnalyzer::xmlID);

    for (size_t i = 0; i < averageRgbHistograms.size(); i++)
    {
        tinyxml2::XMLElement *node2 = doc.NewElement("Cluster");
        node2->SetAttribute("index", (int)i);
        tinyxml2::XMLElement *node3 = doc.NewElement("Average_Histogram_Values_R");
        tinyxml2::XMLElement *node6 = doc.NewElement("Average_Histogram_Values_G");
        tinyxml2::XMLElement *node7 = doc.NewElement("Average_Histogram_Values_B");
        tinyxml2::XMLElement *node4 = doc.NewElement("Similarity_Thresholds");

        for (int j = 0; j < 256; j++)
        {
            tinyxml2::XMLElement *node5 = doc.NewElement("R");
            node5->SetAttribute("index", j);
            node5->SetAttribute("value", averageRgbHistograms[i].accR[j]);
            node3->InsertEndChild(node5);
        }

        for (int j = 0; j < 256; j++)
        {
            tinyxml2::XMLElement *node5 = doc.NewElement("G");
            node5->SetAttribute("index", j);
            node5->SetAttribute("value", averageRgbHistograms[i].accG[j]);
            node6->InsertEndChild(node5);
        }

        for (int j = 0; j < 256; j++)
        {
            tinyxml2::XMLElement *node5 = doc.NewElement("B");
            node5->SetAttribute("index", j);
            node5->SetAttribute("value", averageRgbHistograms[i].accB[j]);
            node7->InsertEndChild(node5);
        }

        tinyxml2::XMLElement * node8 = doc.NewElement("Similarity_Threshold");
        node8->SetAttribute("R", this->minThresR[i]);
        node4->InsertEndChild(node8);

        node8 = doc.NewElement("Similarity_Threshold");
        node8->SetAttribute("G", this->minThresG[i]);
        node4->InsertEndChild(node8);

        node8 = doc.NewElement("Similarity_Threshold");
        node8->SetAttribute("B", this->minThresB[i]);
        node4->InsertEndChild(node8);

        node2->InsertEndChild(node3);
        node2->InsertEndChild(node6);
        node2->InsertEndChild(node7);
        node2->InsertEndChild(node4);

        node1->InsertEndChild(node2);
    }

    doc.LinkEndChild(node1);

    doc.SaveFile(filepath.c_str());

    return false;
}

bool ColourStatisticsAnalyzer::readFromFile(std::string filepath)
{
    //XMLDocument doc;
    //doc.LoadFile( "resources/dream.xml" );
    //return doc.ErrorID();

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError eResult = doc.LoadFile(filepath.c_str());
    if (eResult != tinyxml2::XML_SUCCESS) throw std::runtime_error("Could not parse the supplied XML document. Check the formatting in your browser.");

    tinyxml2::XMLElement* titleElement = doc.FirstChildElement( "Texture_Classifier_Training_Data" );

    if (titleElement == NULL) throw std::runtime_error("Invalid title element in XML document.");

    const char* cID;
    cID = titleElement->Attribute("classifierID");

    std::string strID = std::string(cID);

    if (strID != ColourStatisticsAnalyzer::xmlID) throw std::runtime_error("Attempted to parse training data from the wrong classifier.");

    this->averageRgbHistograms.clear();
    this->minThresR.clear();
    this->minThresG.clear();
    this->minThresB.clear();

    tinyxml2::XMLElement* clusterPntr = titleElement->FirstChildElement("Cluster");

    if (clusterPntr == NULL) throw std::runtime_error("Invalid title element in XML document.");

    for (clusterPntr;
         clusterPntr != NULL;
         clusterPntr = clusterPntr->NextSiblingElement())
    {
        RgbHistogram histo;
        //read the histogram values
        tinyxml2::XMLElement* histoRPtr, *histoGPtr, *histoBPtr, *simPtr;



        histoRPtr = clusterPntr->FirstChildElement("Average_Histogram_Values_R");
        if (histoRPtr == NULL) throw std::runtime_error("Could not find the Average_Histogram_Values_R element.");

        histoGPtr = clusterPntr->FirstChildElement("Average_Histogram_Values_G");
        if (histoGPtr == NULL) throw std::runtime_error("Could not find the Average_Histogram_Values_G element.");

        histoBPtr = clusterPntr->FirstChildElement("Average_Histogram_Values_B");
        if (histoBPtr == NULL) throw std::runtime_error("Could not find the Average_Histogram_Values_B element.");

        simPtr=clusterPntr->FirstChildElement("Similarity_Thresholds");
        if (simPtr == NULL) throw std::runtime_error("Could not find the Similarity_Thresholds element.");



        int counter = 0;
        int tmpIndex =0;
        for (tinyxml2::XMLElement* rValPtr =histoRPtr->FirstChildElement("R");
             rValPtr != NULL;
             rValPtr = rValPtr->NextSiblingElement())
        {
            rValPtr->QueryIntAttribute("index",&tmpIndex);
            if (tmpIndex != counter) throw std::runtime_error("An index for a histogram is wrong.");
            rValPtr->QueryIntAttribute("value", &(histo.accR[counter]));
            counter++;
        }

        if (counter != 256)
            throw std::runtime_error("There are not enough histogram r values in a cluster.");

        counter = 0;

        for (tinyxml2::XMLElement* gValPtr =histoGPtr->FirstChildElement("G");
             gValPtr != NULL;
             gValPtr = gValPtr->NextSiblingElement())
        {
            gValPtr->QueryIntAttribute("index",&tmpIndex);
            if (tmpIndex != counter) throw std::runtime_error("An index for a histogram is wrong.");
            gValPtr->QueryIntAttribute("value", &(histo.accG[counter]));
            counter++;
        }
        if (counter != 256)
            throw std::runtime_error("There are not enough histogram g values in a cluster.");

        counter = 0;

        for (tinyxml2::XMLElement* bValPtr =histoBPtr->FirstChildElement("B");
             bValPtr != NULL;
             bValPtr = bValPtr->NextSiblingElement())
        {
            bValPtr->QueryIntAttribute("index",&tmpIndex);
            if (tmpIndex != counter) throw std::runtime_error("An index for a histogram is wrong.");
            bValPtr->QueryIntAttribute("value", &(histo.accB[counter]));
            counter++;
        }

        if (counter != 256)
            throw std::runtime_error("There are not enough histogram b values in a cluster.");

        averageRgbHistograms.push_back(histo);

        tinyxml2::XMLElement* simValPtr = simPtr->FirstChildElement("Similarity_Threshold");

        minThresR.push_back(0);
        simValPtr->QueryIntAttribute("R", &minThresR.back());


        simValPtr = simValPtr->NextSiblingElement();

        minThresG.push_back(0);
        simValPtr->QueryIntAttribute("G", &minThresG.back());


        simValPtr = simValPtr->NextSiblingElement();

        minThresB.push_back(0);
        simValPtr->QueryIntAttribute("B", &minThresB.back());

    }

    return true;
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
