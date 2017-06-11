#include "ImageSequence.h"



ImageSequence::ImageSequence(std::string _directory)
{
	directory = _directory;

	images.clear();
	imageFileNames.clear();
	std::vector<std::string> files = SystemCall::get_all_files_names_within_folder(directory);

	int filesLength = files.size();
	for (int i = 0; i < filesLength; i++)
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

cv::Mat ImageSequence::imageAt(int index)
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

int ImageSequence::getimageCount()
{
	return images.size();
}

std::string ImageSequence::getDirectory()
{
	return directory;
}

//Generates an ImageSequence consisting of a pseudorandom sequence of (numElements)
//images from the supplied ImageSeqeunce.
ImageSequence ImageSequence::getRandomImageSequence(ImageSequence seq, int numElements)
{
	int randIndex;
	std::vector<cv::Mat> images;
	std::vector<std::string> imageFileNames;
	std::string directory;

	vector<int> indexSet;

	for (int i = 0; i < seq.getimageCount(); i++)
		indexSet.push_back(i);

	directory = seq.getDirectory();

	for (int i = 0; i < numElements; i++)
	{
		randIndex = rand() % indexSet.size();

		int seqIndex = indexSet[randIndex];

		images.push_back(seq.imageAt(seqIndex));
		imageFileNames.push_back(seq.imageFileNameAt(seqIndex));
		indexSet.erase(indexSet.begin() + randIndex);
	}

	ImageSequence newImageSequence(images, imageFileNames, directory);

	return newImageSequence;

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
