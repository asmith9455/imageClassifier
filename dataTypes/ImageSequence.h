#pragma once
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <string>
#include <SystemCall.h>

class ImageSequence
{
private:
	std::vector<cv::Mat> images;
	string directory;
	std::vector<std::string> imageFileNames;

public:
	ImageSequence(std::string directory);
	ImageSequence(std::vector<cv::Mat> _images, std::vector<std::string> _imageFileNames, std::string directory);
    ImageSequence();

    std::vector<std::string> associatedProperties;
    cv::Mat imageAt(size_t index);
	std::string imagePathAt(int index);
	std::string imageFileNameAt(int index);
    size_t getimageCount();
	std::string getDirectory();
    void addImg(cv::Mat img);
    void addImgs(ImageSequence imseq);

    static std::pair<ImageSequence, ImageSequence>
    getRandomImageSequence
    (ImageSequence seq, double fractionToPreserve, bool setSeed, unsigned int randSeed);

	static bool imagesAreEqual(cv::Mat im1, cv::Mat im2);
	static bool imagesSequencesAreEqual(ImageSequence im1, ImageSequence im2);
};
