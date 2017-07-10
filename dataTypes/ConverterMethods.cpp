#include "ConverterMethods.h"

ConverterMethods::ConverterMethods()
{}


QString ConverterMethods::getStringFromContour(std::vector<Point2<int>> vec)
{
    std::vector<std::vector<Point2<int>>> tmpVec;
    tmpVec.push_back(vec);

    return getStringFromContourVector(tmpVec);
}

cv::Mat ConverterMethods::getOpenCvGreyMatFromImageSequences(std::vector<ImageSequence> seqs, int tileWidth, int tileHeight)
{
    int numImgs = 0;
    for(ImageSequence is : seqs)
        numImgs+=is.getimageCount();

    int pixelsPerImg = tileWidth*tileHeight;

    cv::Mat trainingImages(numImgs, pixelsPerImg, CV_32FC1);

    int ctr = 0;
    int column = 0;
    for (ImageSequence is : seqs)
    {
        for(size_t i = 0; i < is.getimageCount(); i++)
        {
            cv::Mat img = is.imageAt(i);
            cv::cvtColor(img, img, cv::COLOR_RGB2GRAY);
            for (int j = 0; j<img.rows; j++)
                for (int k = 0; k < img.cols; k++)
                {
                    //uchar val = img.at<cv::Vec3b>(j,k)[0];
                    uchar val = img.at<uchar>(j,k);
                    trainingImages.at<float>(ctr, column) = val;
                    column++;
                }
            ctr++;
            column = 0;
        }
    }

    return trainingImages;
}

cv::Mat ConverterMethods::getOpenCvXyzMatFromImageSequences(std::vector<ImageSequence> seqs, int tileWidth, int tileHeight)
{
    int numImgs = 0;
    for(ImageSequence is : seqs)
        numImgs+=is.getimageCount();

    int pixelsPerImg = tileWidth*tileHeight;

    cv::Mat trainingImages(numImgs, pixelsPerImg*3, CV_32FC1);

    int ctr = 0;
    int column = 0;
    for (ImageSequence is : seqs)
    {
        for(size_t i = 0; i < is.getimageCount(); i++)
        {
            cv::Mat img = is.imageAt(i);
            for (int j = 0; j<img.rows; j++)
                for (int k = 0; k < img.cols; k++)
                {
                    //uchar val = img.at<cv::Vec3b>(j,k)[0];
                    cv::Vec3b val = img.at<cv::Vec3b>(j,k);
                    trainingImages.at<float>(ctr, column*3) = val[0];
                    trainingImages.at<float>(ctr, column*3+1) = val[1];
                    trainingImages.at<float>(ctr, column*3+2) = val[2];
                    column++;
                }
            ctr++;
            column = 0;
        }
    }

    return trainingImages;
}

cv::Mat ConverterMethods::get1dMatGreyFrom2dMat(cv::Mat mat2d)
{
    cv::Mat arrayFrom2D(1, mat2d.rows*mat2d.cols, CV_32FC1);

    int column = 0;

    cv::cvtColor(mat2d, mat2d, cv::COLOR_RGB2GRAY);
    for (int j = 0; j<mat2d.rows; j++)
        for (int k = 0; k < mat2d.cols; k++)
        {
            //uchar val = img.at<cv::Vec3b>(j,k)[0];
            uchar val = mat2d.at<uchar>(j,k);
            arrayFrom2D.at<float>(0, column) = val;
            column++;
        }

    return arrayFrom2D;
}

cv::Mat ConverterMethods::get1dMatXyzFrom2dMat(cv::Mat mat2d)
{
    cv::Mat arrayFrom2D(1, mat2d.rows*mat2d.cols*3, CV_32FC1);

    int column = 0;

    for (int j = 0; j<mat2d.rows; j++)
        for (int k = 0; k < mat2d.cols; k++)
        {
            //uchar val = img.at<cv::Vec3b>(j,k)[0];
            cv::Vec3b val = mat2d.at<cv::Vec3b>(j,k);
            arrayFrom2D.at<float>(0, column*3) = val[0];
            arrayFrom2D.at<float>(0, column*3+1) = val[1];
             arrayFrom2D.at<float>(0, column*3+2) = val[2];
            column++;
        }

    return arrayFrom2D;
}

QString ConverterMethods::getStringFromContourVector(std::vector<std::vector<Point2<int>>> vec)
{
    QString tmp = "[";
    QString tmp2;
    for(size_t i = 0; i < vec.size(); i++)
    {

        tmp += "[";
        tmp2 = "";

        for(size_t j = 0; j < vec[i].size(); j++)
        {
            tmp2 += "[" + QString::number(vec[i][j].x) + "," + QString::number(vec[i][j].y) +"]";
        }
        tmp += tmp2;

        tmp += "]";
    }
    tmp +="]";
    return tmp;
}

std::vector<Point2<int>> ConverterMethods::getContourFromString(QString qstr)
{
    std::vector<std::vector<Point2<int>>> vec = getContourVectorFromString(qstr);
    return vec[0];
}

std::vector<std::vector<Point2<int>>> ConverterMethods::getContourVectorFromString(QString qstr)
    {
        std::vector<std::vector<Point2<int>>> vec;
        int depth = 0;
        std::string tmp = "";
        int xTmp, yTmp;

        std::string str = qstr.toStdString();

        enum mode { readingX, readingY, parsingSyms };

        mode pMode = parsingSyms;

        for(size_t i = 0; i < str.size(); i++)
        {
            char c = str.at(i);
            if (c == '[')
            {
                depth++;
                if (depth == 2)
                    vec.push_back(std::vector<Point2<int>>());
            }
            else if (c==']')
            {
                depth--;

            }
            if (depth == 0)
                break;

            if(pMode == readingX)
            {
                if(c == ',')
                {
                    xTmp = std::stoi(tmp);
                    pMode = readingY;
                    tmp = "";
                    continue;
                }
                else
                    tmp += c;
            }

            if(pMode == readingY)
            {
                if(depth == 2)
                {
                    yTmp = std::stoi(tmp);
                    vec.back().push_back(Point2<int>(xTmp, yTmp));
                    pMode = parsingSyms;
                    tmp = "";
                }
                else
                    tmp += c;
            }

            if(depth == 3 && pMode == parsingSyms)
                pMode = readingX;

        }
        return vec;
    }



Contour<int> ConverterMethods::getContourClassFromString(QString qstr)
{
    return Contour<int>(getContourFromString(qstr));
}


std::vector<Contour<int>> ConverterMethods::getContourClassVectorFromString(QString qstr)
{
    std::vector<std::vector<Point2<int>>> tmp = getContourVectorFromString(qstr);
    std::vector<Contour<int>> cs;
    for (size_t i = 0; i < tmp.size(); i++)
        cs.push_back( Contour<int>(tmp[i]) );

    return cs;
}

std::shared_ptr<TextureClassifier> ConverterMethods::getClassifierFromFile(std::string filepath)
{
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError eResult = doc.LoadFile(filepath.c_str());
    if (eResult != tinyxml2::XML_SUCCESS) throw std::runtime_error("Could not parse the supplied XML document. Check the formatting in your browser.");



    tinyxml2::XMLElement* titleElement = doc.FirstChildElement( "Texture_Classifier_Training_Data" );

    std::shared_ptr<TextureClassifier> toRet;

    const char* cID;
    cID = titleElement->Attribute(TextureClassifier::getXmlAttributeName().c_str());

    if (cID == NULL) throw std::runtime_error("Classifier attribute not found (cannot determine which type of classifier this is).");

    std::string strID = std::string(cID);

    if (strID == ColourStatisticsAnalyzerRgb::getXmlID())
    {
        std::shared_ptr<ColourStatisticsAnalyzerRgb> tmp = std::make_shared<ColourStatisticsAnalyzerRgb>();
        tmp->readFromFile(filepath);
        toRet = static_pointer_cast<TextureClassifier>(tmp);
    }
    else if (strID == CSA_RGB_WN_v1::getXmlID())
    {
        std::shared_ptr<CSA_RGB_WN_v1> tmp = std::make_shared<CSA_RGB_WN_v1>();
        tmp->readFromFile(filepath);
        toRet = static_pointer_cast<TextureClassifier>(tmp);
    }

    return toRet;
}
