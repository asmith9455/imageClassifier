#include "ImageSequence.h"

ImageSequence::ImageSequence(){}

ImageSequence::ImageSequence(std::string _directory)
{
	directory = _directory;

	images.clear();
	imageFileNames.clear();
	std::vector<std::string> files = SystemCall::get_all_files_names_within_folder(directory);

    size_t filesLength = files.size();
    for (size_t i = 0; i < filesLength; i++)
	{
		std::string filename = files[i];
		std::string path = directory + "/" + filename;
		cv::Mat tmp = cv::imread(path, cv::IMREAD_COLOR);
		images.push_back(tmp);
		imageFileNames.push_back(filename);
	}

}

ImageSequence::ImageSequence(std::vector<cv::Mat> _images, std::vector<std::string> _imageFileNames, std::string _directory)
{
	images = _images;
	imageFileNames = _imageFileNames;
	directory = _directory;
}

cv::Mat ImageSequence::imageAt(size_t index)
{
	return images[index];
}

std::string ImageSequence::imagePathAt(int index)
{
	return directory + "\\" + imageFileNames[index];
}

std::string ImageSequence::imageFileNameAt(int index)
{
	return imageFileNames[index];
}

size_t ImageSequence::getimageCount()
{
	return images.size();
}

std::string ImageSequence::getDirectory()
{
	return directory;
}

void ImageSequence::addImg(cv::Mat img)
{
    this->images.push_back(img);
}

void ImageSequence::addImgs(ImageSequence imseq)
{
    for (size_t i = 0; i < imseq.getimageCount(); i++)
        this->images.push_back(imseq.imageAt(i));
}

//Generates an ImageSequence consisting of a pseudorandom sequence of (numElements)
//images from the supplied ImageSeqeunce.
std::pair<ImageSequence, ImageSequence>
ImageSequence::getRandomImageSequence
(ImageSequence seq, double fractionToPreserve, bool setSeed, unsigned int randSeed)
{
    if (setSeed)
        srand (randSeed);


    std::pair<ImageSequence, ImageSequence> toRet;
    size_t randIndex;
    size_t seqIndex;
    double numElementsD = (fractionToPreserve * (double)seq.getimageCount());
    size_t numElements = (size_t)numElementsD;  //TODO: adjust this so that no double conversion is required (only a problem for huge datasets)

    vector<size_t> indexSet;

    for (size_t i = 0; i < seq.getimageCount(); i++)
		indexSet.push_back(i);


    for (size_t i = 0; i < numElements; i++)
	{
		randIndex = rand() % indexSet.size();
        seqIndex = indexSet[randIndex];

        toRet.first.addImg(seq.imageAt(seqIndex));
		indexSet.erase(indexSet.begin() + randIndex);
	}

    for (size_t i = 0; i < indexSet.size(); i++)
    {
        seqIndex = indexSet[i];
        toRet.second.addImg(seq.imageAt(seqIndex));
    }


    return toRet;

}


bool ImageSequence::imagesAreEqual(cv::Mat im1, cv::Mat im2)
{
	if (im1.rows != im2.rows || im1.cols != im2.cols)
		return false;

	bool match = true;

	for (int i = 0; i < im1.rows; i++)
	{
		for (int j = 0; j < im1.cols; j++)
		{
			match = match && im1.at<cv::Vec3b>(i, j)[0] == im2.at<cv::Vec3b>(i, j)[0];
			match = match && im1.at<cv::Vec3b>(i, j)[1] == im2.at<cv::Vec3b>(i, j)[1];
			match = match && im1.at<cv::Vec3b>(i, j)[2] == im2.at<cv::Vec3b>(i, j)[2];
		}
	}

	return match;
}

bool ImageSequence::imagesSequencesAreEqual(ImageSequence im1, ImageSequence im2)
{
	if (im1.getimageCount() != im2.getimageCount())
		return false;

	bool match = true;
	for (int i = 0; i < im1.getimageCount(); i++)
		match = match && ImageSequence::imagesAreEqual(im1.imageAt(i), im2.imageAt(i));

	return match;
}
