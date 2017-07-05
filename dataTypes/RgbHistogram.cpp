#include "RgbHistogram.h"
#include <algorithm>

RgbHistogram::RgbHistogram()
{
    accR = std::vector<int>(256,0);
    accG = std::vector<int>(256,0);
    accB = std::vector<int>(256,0);
}

RgbHistogram::RgbHistogram(Mat img)
{
    accR = std::vector<int>(256,0);
    accG = std::vector<int>(256,0);
    accB = std::vector<int>(256,0);

	fillAccumulators(img);
}

RgbHistogram::RgbHistogram(int _accR[], int _accG[], int _accB[])
{
    accR = std::vector<int>(256,0);
    accG = std::vector<int>(256,0);
    accB = std::vector<int>(256,0);

	for (int i = 0; i < 256; i++)
	{
		accR[i] = _accR[i];
		accG[i] = _accG[i];
		accB[i] = _accB[i];
	}
	
}

void RgbHistogram::fillAccumulators(Mat img)
{

    meanBinB = 0;
    meanBinR = 0;
    meanBinG = 0;

    numR = 0;
    numG = 0;
    numB = 0;

	for (int i = 0; i < img.rows; i++)
	{
		for (int j = 0; j < img.cols; j++)
		{
            meanBinR += img.at<Vec3b>(i, j)[0];
            meanBinB += img.at<Vec3b>(i, j)[1];
            meanBinG += img.at<Vec3b>(i, j)[2];

            numR++;
            numG++;
            numB++;

            accR[img.at<Vec3b>(i, j)[0]]++; //all images stored in the database are rgb
			accG[img.at<Vec3b>(i, j)[1]]++;
            accB[img.at<Vec3b>(i, j)[2]]++;
		}
	}

    meanBinR = meanBinR / numR;
    meanBinG = meanBinG / numG;
    meanBinB = meanBinB / numB;

}

//rotate RGB components by the same amount, so that the
//mean bin of the R component is as specified.
void RgbHistogram::rotateMeanUsingRed(int newBinForMeanR)
{
    int leftShiftR = meanBinR - newBinForMeanR;

    if (leftShiftR == 0)
        return;

    if (leftShiftR > 0)
        rotateLeft(leftShiftR);
    else
        rotateRight(0-leftShiftR);
}

void RgbHistogram::rotateLeft(int rotateAmountLeft)
{
    std::rotate(accR.begin(), accR.begin() + rotateAmountLeft, accR.end());
    std::rotate(accG.begin(), accG.begin() + rotateAmountLeft, accG.end());
    std::rotate(accB.begin(), accB.begin() + rotateAmountLeft, accB.end());
}

void RgbHistogram::rotateRight(int rotateAmountRight)
{
    std::rotate(accR.rbegin(), accR.rbegin()+rotateAmountRight, accR.rend());
    std::rotate(accG.rbegin(), accG.rbegin()+rotateAmountRight, accG.rend());
    std::rotate(accB.rbegin(), accB.rbegin()+rotateAmountRight, accB.rend());
}

int RgbHistogram::getSumAbsDifferenceR(RgbHistogram h)
{
    int sumAbsDiffR = 0;

    for(int i = 0; i < 256; i++)
        sumAbsDiffR += abs(accR[i] - h.accR[i]);

    return sumAbsDiffR;
}

int RgbHistogram::getSumAbsDifferenceG(RgbHistogram h)
{
    int sumAbsDiffG = 0;

    for(int i = 0; i < 256; i++)
        sumAbsDiffG += abs(accG[i] - h.accG[i]);

    return sumAbsDiffG;
}

int RgbHistogram::getSumAbsDifferenceB(RgbHistogram h)
{
    int sumAbsDiffB = 0;

    for(int i = 0; i < 256; i++)
        sumAbsDiffB += abs(accB[i] - h.accB[i]);

    return sumAbsDiffB;
}

vector<RgbHistogram> RgbHistogram::getHistograms(ImageSequence imgSeq)
{
	vector<RgbHistogram> vecs;
	
	for (int i = 0; i < imgSeq.getimageCount(); i++)
	{
		
		RgbHistogram histo(imgSeq.imageAt(i));
		
		vecs.push_back(histo);
	}

	return vecs;
}

vector<RgbHistogram> RgbHistogram::averageRgbHistogramSequence(vector<RgbHistogram> rgbHistogramSequence)
{
	int histogramCount = rgbHistogramSequence.size();
	vector<RgbHistogram> tmpHistoVect;
	if (histogramCount == 0)
		return tmpHistoVect;
	//double histogramCountD = (double)histogramCount;

	int sumR[256] = { 0 };
	int sumG[256] = { 0 };
	int sumB[256] = { 0 };


	for (int i = 0; i < 256; i++)
	{
		for (int j = 0; j < histogramCount; j++)
		{
			sumR[i] += rgbHistogramSequence[j].accR[i];
			sumG[i] += rgbHistogramSequence[j].accG[i];
			sumB[i] += rgbHistogramSequence[j].accB[i];
		}	
	}

	int accR[256];
	int accG[256];
	int accB[256];

	for (int i = 0; i < 256; i++)
	{
		accR[i] = sumR[i] / histogramCount;
		accG[i] = sumG[i] / histogramCount;
		accB[i] = sumB[i] / histogramCount;
	}

	RgbHistogram tmpHisto(accR, accG, accB);

	
	tmpHistoVect.push_back(tmpHisto);

	return tmpHistoVect;
}

void RgbHistogram::writeHistosToFile(vector<vector<RgbHistogram>> histos, string path)
{
	ofstream myfile;
	myfile.open(path);
	
	for (int i = 0; i < histos.size(); i++)
	{
		for (int k = 0; k < histos[i].size(); k++)
		{
			for (int j = 0; j < 256; j++)
			{
				myfile << j << ", "
					<< histos[i][k].accR[j] << ", "
					<< histos[i][k].accG[j] << ", "
					<< histos[i][k].accB[j]
					<< "\n";
			}
		}
		
	}

	myfile.close();
}

void RgbHistogram::writeHistosToFile(vector<RgbHistogram> histos, string path)
{
	ofstream myfile;
	myfile.open(path);

	for (int i = 0; i < histos.size(); i++)
	{
		
		for (int j = 0; j < 256; j++)
		{
			myfile << j << ", "
				<< histos[i].accR[j] << ", "
				<< histos[i].accG[j] << ", "
				<< histos[i].accB[j]
				<< "\n";
		}

	}

	myfile.close();
}

int RgbHistogram::similarityR(RgbHistogram h1, RgbHistogram h2)
{	
	int similarityR = 0;

	for (int i = 0; i < 256; i++)
		similarityR += h1.accR[i] * h2.accR[i];

	return similarityR;
}

int RgbHistogram::similarityG(RgbHistogram h1, RgbHistogram h2)
{
	int similarityG = 0;

	for (int i = 0; i < 256; i++)
		similarityG += h1.accG[i] * h2.accG[i];

	return similarityG;
}

int RgbHistogram::similarityB(RgbHistogram h1, RgbHistogram h2)
{
	int similarityB = 0;

	for (int i = 0; i < 256; i++)
		similarityB += h1.accB[i] * h2.accB[i];

	return similarityB;
}


bool RgbHistogram::AreEqual(RgbHistogram h1, RgbHistogram h2)
{
	bool equal = true, b1, b2, b3, tmpEqual;

	for (int i = 0; i < 256; i++)
	{
		b1 = h1.accR[i] == h2.accR[i];
		b2 = h1.accG[i] == h2.accG[i];
		b3 = h1.accB[i] == h2.accB[i];

		tmpEqual = b1 && b2 && b3;

		equal = equal && tmpEqual;
        if (!equal)
            cout << "ne" << std::endl;
	}

	return equal;
}

bool RgbHistogram::ListIsEqual(vector<RgbHistogram> h1, vector<RgbHistogram> h2)
{
	bool equal = true;

	for (int i = 0; i < h1.size(); i++)
	{
		bool tmp = RgbHistogram::AreEqual(h1[i], h2[i]);
		equal = equal && tmp;
	}

	return equal;
}

bool RgbHistogram::ListOfListIsEqual(vector<vector<RgbHistogram>> h1, vector<vector<RgbHistogram>> h2)
{
	bool equal = true;

	for (int i = 0; i < h1.size(); i++)
	{
		equal = equal && RgbHistogram::ListIsEqual(h1[i], h2[i]);
	}

	return equal;
}
