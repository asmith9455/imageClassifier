#include <CSA_RGB_shiftHisto.h>

CSA_RGB_shiftHisto::CSA_RGB_shiftHisto(){}

CSA_RGB_shiftHisto::CSA_RGB_shiftHisto(
        ImageSequence _imageSequence_target,
        ImageSequence _imageSequence_notTarget,
        double _safetyFactor) : TextureClassifier(ImageSequence(), ImageSequence())
{
    imageSequence_target =  _imageSequence_target;
    imageSequence_notTarget = _imageSequence_notTarget;

    safetyFactor = _safetyFactor;
}

void CSA_RGB_shiftHisto::analyze()
{
	updateConstantImageSize();
    generateRgbHistogramsForImageSequence();
    rotateHistogramMeans();
    generateAverageRgbHistogram();
    generateChangeStats();

    generateHistoThresholds();
}

void CSA_RGB_shiftHisto::rotateHistogramMeans()
{
    for(size_t i = 0; i < rgbHistograms.size(); i++)
    {
        //rotate all the means for the red components to 0
        rgbHistograms[i].rotateMeanUsingRed(0);
    }
}

void CSA_RGB_shiftHisto::generateAverageRgbHistogram()
{
    std::vector<RgbHistogram> tmp = RgbHistogram::averageRgbHistogramSequence(rgbHistograms);
    if (tmp.size() > 0)
        averageRgbHistogram = tmp[0];
}

void CSA_RGB_shiftHisto::generateRgbHistogramsForImageSequence()
{

    rgbHistograms = generateRgbHistogramsForImageSequence(imageSequence_target);

}

void CSA_RGB_shiftHisto::generateChangeStats()
{

    std::vector<float> ratios;

    float minAvg = (float)tileWidth + 2.0;


    float maxAvg = -1.0,
            minStdDev = (float)tileWidth + 2.0,
            maxStdDev = -1.0;
    minRatio = (float)tileWidth + 2.0;
    maxRatio = -1.0;

    for (size_t k = 0; k < imageSequence_target.getimageCount(); k++)
    {
        cv::Mat img = imageSequence_target.imageAt(k);
        std::vector<float> tmpRatios;
        for (int i = 0; i < img.rows; i++)
        {
            //row statistics
            //maybe do ratio of positive changes to ratio of negative changes??

            int prevVal = 0, numPos = 0, numNeg = 0;

            for (int j = 1; j < img.cols; j++)
            {
                int pxVal = img.at<Vec3b>(i, j)[0];

                if (pxVal > prevVal)
                    numPos++;
                else if (pxVal < prevVal)
                    numNeg++;

                //accR[]++; //all images stored in the database are rgb
                //accG[img.at<Vec3b>(i, j)[1]]++;
                //accB[img.at<Vec3b>(i, j)[2]]++;

                prevVal = pxVal;
            }

            float r;
            if (numNeg > 0)
                r = (float) numPos / (float) numNeg;
            else
                r = (float) (this->tileWidth + 1); //"infinity"

            if (r < minRatio)
                minRatio = r;
            if (r > maxRatio)
                maxRatio = r;

            ratios.push_back(r);
            tmpRatios.push_back(r);
        }

        float sum = std::accumulate(std::begin(tmpRatios), std::end(tmpRatios), 0.0);
        float m =  sum / tmpRatios.size();

        float accum = 0.0;
        std::for_each (std::begin(tmpRatios), std::end(tmpRatios), [&](const float d) {
            accum += (d - m) * (d - m);
        });

        double stdev = sqrt(accum / (tmpRatios.size()-1));
    }

    float sum2 = std::accumulate(std::begin(ratios), std::end(ratios), 0.0);
    float m2 =  sum2 / ratios.size();

    float accum2 = 0.0;
    std::for_each (std::begin(ratios), std::end(ratios), [&](const float d) {
        accum2 += (d - m2) * (d - m2);
    });

    double stdev2 = sqrt(accum2 / (ratios.size()-1));

    //use another safety factor or a different method of comparison here - also add some more statistics - this one was very simple and dropped false + rate by 5%
    //minRatio = m2 - 4.0*stdev2;
    //maxRatio = m2 + 4.0*stdev2;

}

void CSA_RGB_shiftHisto::generateChangeThresholds()
{

}

void CSA_RGB_shiftHisto::generateHistoThresholds()
{
    //must be identical - maximum possible similarity
    maxThresR = 0;
    maxThresG = 0;
    maxThresB = 0;

    int sumThresR = 0;
    int sumThresG = 0;
    int sumThresB = 0;

    int tmp;
    for (int j = 0; j < rgbHistograms.size(); j++)
    {
        tmp = averageRgbHistogram.getSumAbsDifferenceR(
                    rgbHistograms[j]);

        if (tmp > maxThresR)
            maxThresR = tmp;

        sumThresR += tmp;

        tmp = averageRgbHistogram.getSumAbsDifferenceG(
                    rgbHistograms[j]);
        if (tmp < maxThresG)
            maxThresG = tmp;

        sumThresG += tmp;

        tmp = averageRgbHistogram.getSumAbsDifferenceB(
                    rgbHistograms[j]);

        if (tmp < maxThresB)
            maxThresB = tmp;

        sumThresB += tmp;
    }

    // TODO: we can probably combine these double conversions with the calculations in the push back to avoid double completely
    //double avgThresR = (double)sumThresR / (double)rgbHistograms.size();
    //double avgThresG = (double)sumThresG / (double)rgbHistograms.size();
    //double avgThresB = (double)sumThresB / (double)rgbHistograms.size();

    //probably do not need safety factor for this method - could add as a selectivity method though
    //maxThresR = ((double)maxThresR - avgThresR)*safetyFactor + (double)maxThresR;
    //maxThresG = ((double)maxThresR - avgThresG)*safetyFactor + (double)maxThresR;
    //maxThresB = ((double)maxThresR - avgThresB)*safetyFactor + (double)maxThresR;



}

vector<RgbHistogram> CSA_RGB_shiftHisto::generateRgbHistogramsForImageSequence(ImageSequence targetImageSequence)
{

	vector<RgbHistogram> vecs;
    vecs = RgbHistogram::getHistograms(targetImageSequence);

	return vecs;

}

bool CSA_RGB_shiftHisto::isTarget(Mat img)
{
	RgbHistogram histo(img);
	int tempR, tempG, tempB;
	bool match = false;


    //iterate through the histograms that represent each pre-cluster of colour histogram data
    histo.rotateMeanUsingRed(0);

    tempR = averageRgbHistogram.getSumAbsDifferenceR(histo);
    tempG = averageRgbHistogram.getSumAbsDifferenceG(histo);
    tempB = averageRgbHistogram.getSumAbsDifferenceB(histo);

    if (tempR <= maxThresR && tempG <= maxThresG && tempB <= maxThresB)
    {
        match = true;
    }

    if (false) //chose to use the change ratio filtering component
    {
        for (int i = 0; i < img.rows; i++)
        {
            //row statistics
            //maybe do ratio of positive changes to ratio of negative changes??

            int prevVal = 0, numPos = 0, numNeg = 0;

            for (int j = 1; j < img.cols; j++)
            {
                int pxVal = img.at<Vec3b>(i, j)[0];

                if (pxVal > prevVal)
                    numPos++;
                else if (pxVal < prevVal)
                    numNeg++;

                //accR[]++; //all images stored in the database are rgb
                //accG[img.at<Vec3b>(i, j)[1]]++;
                //accB[img.at<Vec3b>(i, j)[2]]++;

                prevVal = pxVal;
            }

            float r;
            if (numNeg > 0)
                r = (float) numPos / (float) numNeg;
            else
                r = (float) (this->tileWidth + 1); //"infinity"

            if (!(r > minRatio &&  r < maxRatio))
                match  = false;

        }
    }

	return match;
}

int CSA_RGB_shiftHisto::getTileHeight()
{
	return tileHeight;
}

int CSA_RGB_shiftHisto::getTileWidth()
{
	return tileWidth;
}

bool CSA_RGB_shiftHisto::writeToFile(std::string filepath)
{
    //we need to write all of the averageRgbHistograms and the threshold data for each channel for each of the histograms to file
    //using namespace tinyxml2;

    tinyxml2::XMLDocument doc;

    tinyxml2::XMLDeclaration * decl = doc.NewDeclaration("xml version=\"1.0\" encoding=\"UTF-8\"");
    doc.LinkEndChild( decl );

    tinyxml2::XMLElement *node1 = doc.NewElement("Texture_Classifier_Training_Data");

    node1->SetAttribute(TextureClassifier::getXmlAttributeName().c_str(), CSA_RGB_shiftHisto::getXmlID().c_str());

    tinyxml2::XMLElement *node2 = doc.NewElement("Cluster");
    node2->SetAttribute("index", (int)0);
    tinyxml2::XMLElement *node3 = doc.NewElement("Average_Histogram_Values_R");
    tinyxml2::XMLElement *node6 = doc.NewElement("Average_Histogram_Values_G");
    tinyxml2::XMLElement *node7 = doc.NewElement("Average_Histogram_Values_B");
    tinyxml2::XMLElement *node4 = doc.NewElement("Similarity_Thresholds");

    for (int j = 0; j < 256; j++)
    {
        tinyxml2::XMLElement *node5 = doc.NewElement("R");
        node5->SetAttribute("index", j);
        node5->SetAttribute("value", averageRgbHistogram.accR[j]);
        node3->InsertEndChild(node5);
    }

    for (int j = 0; j < 256; j++)
    {
        tinyxml2::XMLElement *node5 = doc.NewElement("G");
        node5->SetAttribute("index", j);
        node5->SetAttribute("value", averageRgbHistogram.accG[j]);
        node6->InsertEndChild(node5);
    }

    for (int j = 0; j < 256; j++)
    {
        tinyxml2::XMLElement *node5 = doc.NewElement("B");
        node5->SetAttribute("index", j);
        node5->SetAttribute("value", averageRgbHistogram.accB[j]);
        node7->InsertEndChild(node5);
    }

    tinyxml2::XMLElement * node8 = doc.NewElement("Similarity_Threshold");
    node8->SetAttribute("R", this->maxThresR);
    node4->InsertEndChild(node8);

    node8 = doc.NewElement("Similarity_Threshold");
    node8->SetAttribute("G", this->maxThresG);
    node4->InsertEndChild(node8);

    node8 = doc.NewElement("Similarity_Threshold");
    node8->SetAttribute("B", this->maxThresB);
    node4->InsertEndChild(node8);



    node2->InsertEndChild(node3);
    node2->InsertEndChild(node6);
    node2->InsertEndChild(node7);
    node2->InsertEndChild(node4);

    node1->InsertEndChild(node2);

    tinyxml2::XMLElement *node9 = doc.NewElement("Image_Tile_Size");
    node9->SetAttribute("width", this->getTileWidth());
    node9->SetAttribute("height", this->getTileHeight());
    node1->InsertEndChild(node9);
    doc.LinkEndChild(node1);

    doc.SaveFile(filepath.c_str());

    return false;
}

bool CSA_RGB_shiftHisto::readFromFile(std::string filepath)
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
    cID = titleElement->Attribute(TextureClassifier::getXmlAttributeName().c_str());

    if (cID == NULL) throw std::runtime_error("Classifier attribute not found (cannot determine which type of classifier this is).");

    std::string strID = std::string(cID);

    if (strID != CSA_RGB_shiftHisto::getXmlID()) throw std::runtime_error("Attempted to parse training data from the wrong classifier.");


    tinyxml2::XMLElement* clusterPntr = titleElement->FirstChildElement("Cluster");

    if (clusterPntr == NULL) throw std::runtime_error("Invalid title element in XML document.");

    for (clusterPntr;
         clusterPntr != NULL;
         clusterPntr = clusterPntr->NextSiblingElement())
    {
        std::string ptrName = std::string(clusterPntr->Name());
        if (ptrName == "Image_Tile_Size")
            break;

        RgbHistogram histo;
        //read the histogram values
        tinyxml2::XMLElement* histoRPtr, *histoGPtr, *histoBPtr, *simPtr;



        histoRPtr = clusterPntr->FirstChildElement("Average_Histogram_Values_R");
        if (histoRPtr == NULL)
            throw std::runtime_error("Could not find the Average_Histogram_Values_R element.");

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

        averageRgbHistogram = histo;

        tinyxml2::XMLElement* simValPtr = simPtr->FirstChildElement("Similarity_Threshold");

        simValPtr->QueryIntAttribute("R", &maxThresR);


        simValPtr = simValPtr->NextSiblingElement();

        simValPtr->QueryIntAttribute("G", &maxThresG);


        simValPtr = simValPtr->NextSiblingElement();

        simValPtr->QueryIntAttribute("B", &maxThresB);

    }

    int width=0, height=0;
    tinyxml2::XMLElement* imgSizePtr = titleElement->FirstChildElement("Image_Tile_Size");
    imgSizePtr->QueryIntAttribute("width", &width);
    imgSizePtr->QueryIntAttribute("height", &height);
    this->setTileSize(height, width);

    return true;
}

void CSA_RGB_shiftHisto::setTileSize(int height, int width)
{
    this->tileWidth = width;
    this->tileHeight = height;
}

void CSA_RGB_shiftHisto::updateConstantImageSize()
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

std::string CSA_RGB_shiftHisto::getXmlID()
{
    return "CSA_RGB_shiftHisto";
}
