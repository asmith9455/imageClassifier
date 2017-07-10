#include <IMG_SVM.h>

IMG_SVM::IMG_SVM(){}

IMG_SVM::IMG_SVM(
        ImageSequence _imageSequence_target,
        ImageSequence _imageSequence_notTarget,
        ColourMode _colourMode) : TextureClassifier(_imageSequence_target, _imageSequence_notTarget)
{
    colourMode = _colourMode;
}

void IMG_SVM::analyze()
{
	updateConstantImageSize();

    //create labels

    std::vector<int> labels;
    for(size_t i = 0; i < imageSequence_target.getimageCount(); i++)
        labels.push_back(1);
    for(size_t i = 0; i < imageSequence_notTarget.getimageCount(); i++)
        labels.push_back(0);

    cv::Mat labelsMat(labels);

    std::vector<ImageSequence> tmp = { imageSequence_target, imageSequence_notTarget};

    cv::Mat trainingImages;

    if (colourMode == ColourMode::GREY)
        trainingImages = ConverterMethods::getOpenCvGreyMatFromImageSequences(tmp, getTileWidth(), getTileHeight());
    else if (colourMode == ColourMode::RGB)
        trainingImages = ConverterMethods::getOpenCvXyzMatFromImageSequences(tmp, getTileWidth(), getTileHeight());
    else
        throw std::runtime_error("unrecognized colour mode");

    this->svm = cvSVM::create();
    svm->setType(cvSVM::C_SVC);
    svm->setKernel(cvSVM::RBF);
    svm->setTermCriteria(cv::TermCriteria(cv::TermCriteria::MAX_ITER, 10000, 1e-6));

    cv::Ptr<cv::ml::TrainData> td =
            cv::ml::TrainData::create(trainingImages, cv::ml::ROW_SAMPLE, labelsMat);

    svm->train(td);
}

bool IMG_SVM::isTarget(cv::Mat img)
{
    cv::Mat mat1d;

    if (colourMode == ColourMode::GREY)
        mat1d = ConverterMethods::get1dMatGreyFrom2dMat(img);
    else if (colourMode == ColourMode::RGB)
        mat1d = ConverterMethods::get1dMatXyzFrom2dMat(img);
    else
        throw std::runtime_error("unrecognized colour mode");

    float result = svm->predict(mat1d);

    if (result == 1.0)
        return true;
    else if (result == 0.0)
        return false;
    else
        return true;
}

int IMG_SVM::getTileHeight()
{
	return tileHeight;
}

int IMG_SVM::getTileWidth()
{
	return tileWidth;
}

bool IMG_SVM::writeToFile(std::string filepath)
{
    return false;
}

bool IMG_SVM::readFromFile(std::string filepath)
{
    return false;
}

void IMG_SVM::setTileSize(int height, int width)
{
    this->tileWidth = width;
    this->tileHeight = height;
}

void IMG_SVM::updateConstantImageSize()
{
	int constantSize = -1;
	bool constantBoolW = true, constantBoolH = true;
	int rows, cols;

    if (imageSequences_target.size() > 0 && imageSequences_target[0].getimageCount() > 0)
	{
        rows = imageSequences_target[0].imageAt(0).rows;
        cols = imageSequences_target[0].imageAt(0).cols;
        for (int i = 0; i < imageSequences_target.size(); i++)
            for (int j = 0; j < imageSequences_target[i].getimageCount(); j++)
            {
                constantBoolH = constantBoolH && imageSequences_target[i].imageAt(j).rows == rows;
                constantBoolW = constantBoolW && imageSequences_target[i].imageAt(j).cols == cols;
            }
	}
    else if (imageSequence_target.getimageCount() > 0)
    {
        rows = imageSequence_target.imageAt(0).rows;
        cols = imageSequence_target.imageAt(0).cols;
    }


	if (constantBoolH)
		tileHeight = rows;

	if (constantBoolW)
		tileWidth = cols;
}

std::string IMG_SVM::getXmlID()
{
    return "IMG_SVM";
}
