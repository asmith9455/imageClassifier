#include <generalFunc.h>

// performs a shift right operation on a std:vector and inserts a value into the open position
std::vector<double> shift_right_insert(std::vector<double> vect, double val)
{
	std::vector<double> storage(vect.size());
	for (int i = 0; i < (vect.size() - 1); i++)
	{
		storage[i + 1] = vect[i];
	}

	storage[0] = val;
	return storage;
}

// implements a delay in msec
void delay(unsigned int msec)
{
	clock_t target = msec + clock();
	while (target > clock());
}

// this function allows for pyrDown to be called multiple times on the same image with a single line
cv::Mat pyrDownDef(cv::Mat imIN, int times)
{
	cv::Mat imTemp;
	for (int i = 1; i < times; i++)
	{
		if (i == 1)
			cv::pyrDown(imIN, imTemp); // apply first pyrDown
		else
			cv::pyrDown(imTemp, imTemp); // apply remaining pyrDown
	}
	return imTemp;
}
