#include "stop_marker_detection.h"

// Draw Bounding Box and Distance of Largest Contour from Camera on outImg
void drawMaxRectDist(cv::Mat binImg, cv::Mat& outImg, double objWidth, double foclen, int tilesize)
{
	// binImg is small input image from classifer (needs to be 162x91)
	// outImg is large image for viewing and deriving distance (needs to be 2592x1458)
	std::vector<std::vector<cv::Point>> contour_arr;	// store detected contours

	// Step 1: Find contours in image
		cv::findContours(binImg.clone(), contour_arr, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE); // find all contours in image
		if (contour_arr.size() == 0) return; // check for no detected contours to prevent exception
		std::vector<cv::Rect> detectBB(contour_arr.size());	// vector of detected boundry boxes

	// Step 2: Find Bounds of Max Area Rectangle
		for (int i = 0; i < contour_arr.size(); i++) detectBB[i] = cv::boundingRect(contour_arr[i]); // convert all contours to bounding boxes
		int maxAreaIndex = maxRectAreaIndex(detectBB); // find rectangle with max area

	// Step 3: Draw Bounding Box on Image
		cv::Rect scaledRect(detectBB[maxAreaIndex].x*tilesize, detectBB[maxAreaIndex].y*tilesize, detectBB[maxAreaIndex].width*tilesize, detectBB[maxAreaIndex].height*tilesize); // scale detect rectangle to new resolution for distance calc
		cv::rectangle(outImg, scaledRect, cv::Scalar(255, 0, 0), 6, cv::LINE_8, 0); // Draw Bounding Rectangle on Image

	// Step 4: Find Distance from Image and Draw it On the Target
		double objDist = objDetectDist(objWidth, scaledRect.width, foclen); // get distance from camera
		cv::putText(outImg, std::to_string(objDist), cv::Point(scaledRect.x + scaledRect.width, scaledRect.y), cv::FONT_HERSHEY_SIMPLEX, 3, cv::Scalar(0, 0, 225), 8, 8); // print on image
	
	////View image debug use only
	//	pyrDown(outImg, outImg); pyrDown(outImg, outImg); //scale image for viewing
	//	cv::imshow("Im", outImg);
	//	cv::waitKey(0);
	//	cv::destroyAllWindows(); // Close All Windows
}

// Compute Largest Contour's Distance from Camera (with option to draw bounding rectangle and distance in meters on image)
double getMaxRectDist(cv::Mat binImg, cv::Mat& outImg, double objWidth, double foclen, int tilesize, bool drawRect, bool drawDist)
{
	// binImg is small input image from classifer (needs to be 162x91)
	// outImg is large image for viewing and deriving distance (needs to be 2592x1458)
	std::vector<std::vector<cv::Point>> contour_arr;	// store detected contours

	// Step 1: Find contours in image
		cv::findContours(binImg.clone(), contour_arr, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE); // find all contours in image
		if (contour_arr.size() == 0) return 0;; // check for no detected contours to prevent exception
		std::vector<cv::Rect> detectBB(contour_arr.size());	// vector of detected boundry boxes

	// Step 2: Find Bounds of Max Area Rectangle and Get Distance from Camera
		for (int i = 0; i < contour_arr.size(); i++) detectBB[i] = cv::boundingRect(contour_arr[i]); // convert all contours to bounding boxes
		int maxAreaIndex = maxRectAreaIndex(detectBB); // find rectangle with max area
		cv::Rect scaledRect(detectBB[maxAreaIndex].x*tilesize, detectBB[maxAreaIndex].y*tilesize, detectBB[maxAreaIndex].width*tilesize, detectBB[maxAreaIndex].height*tilesize); // scale detect rectangle to new resolution for distance calc
		double objDist = objDetectDist(objWidth, scaledRect.width, foclen); // get distance from camera

	// Step 3: Draw on Image
		if (drawRect) cv::rectangle(outImg, scaledRect, cv::Scalar(255, 0, 0), 6, cv::LINE_8, 0); // Draw Bounding Rectangle on Image
		if (drawDist) cv::putText(outImg, std::to_string(objDist), cv::Point(scaledRect.x + scaledRect.width, scaledRect.y), cv::FONT_HERSHEY_SIMPLEX, 3, cv::Scalar(0, 0, 225), 8, 8); // print on image

	////View image debug use only
	//	pyrDown(outImg, outImg); pyrDown(outImg, outImg); //scale image for viewing
	//	cv::imshow("Im", outImg);
	//	cv::waitKey(0);
	//	cv::destroyAllWindows(); // Close All Windows

	return objDist;
}

// Compute Largest Contour's Distance from Camera
double getMaxRectDist_NoDraw(cv::Mat binImg, double objWidth, double foclen, int tilesize)
{
	// binImg is small input image from classifer (needs to be 162x91)
	std::vector<std::vector<cv::Point>> contour_arr;	// store detected contours

														// Step 1: Find contours in image
	cv::findContours(binImg.clone(), contour_arr, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE); // find all contours in image
	if (contour_arr.size() == 0) return 0;; // check for no detected contours to prevent exception
	std::vector<cv::Rect> detectBB(contour_arr.size());	// vector of detected boundry boxes

														// Step 2: Find Bounds of Max Area Rectangle and Get Distance from Camera
	for (int i = 0; i < contour_arr.size(); i++) detectBB[i] = cv::boundingRect(contour_arr[i]); // convert all contours to bounding boxes
	int maxAreaIndex = maxRectAreaIndex(detectBB); // find rectangle with max area
	cv::Rect scaledRect(detectBB[maxAreaIndex].x*tilesize, detectBB[maxAreaIndex].y*tilesize, detectBB[maxAreaIndex].width*tilesize, detectBB[maxAreaIndex].height*tilesize); // scale detect rectangle to new resolution for distance calc
	double objDist = objDetectDist(objWidth, scaledRect.width, foclen); // get distance from camera
	return objDist;
}

// Find index of rectangle with max area
int maxRectAreaIndex(std::vector<cv::Rect> vect)
{
	// compute index of rectangle with max area
	int max = vect[0].area();
	int maxIndex = 0;
	int vectsize = vect.size();
	for (int i = 1; i < vectsize; i++)
	{
		if (vect[i].area() > max)
		{
			max = vect[i].area();
			maxIndex = i;
		}
	}
	return maxIndex; // return highest area's index
}

// Derive focal length in pixels when real object width & distance are known along with measured pixel width from image
double f_len_derv(double objWidth, double objDist, double objPixelWidth)
{
	double f_len = (objPixelWidth*objDist) / objWidth;
	return f_len;
}

// Get the approximate distance of a known size object from the camera using a single image with the focal length and object width in pixels known
double objDetectDist(double objWidth, double objPixelWidth, double f_len)
{
	double objDist = (f_len*objWidth) / objPixelWidth;
	return objDist;
}

// round double to a specific number of places
double rndDoubleDown(double num, int places)
{
	double rndNum;
	rndNum = floor(num*pow(10, places)) / pow(10, places);
	std::cout << "Regular: " << num << " After: " << rndNum;
	return rndNum;
}

// Draw Bounding Boxes for All Detected Contours with Numbering
void drawBoundingBoxes_wNumbers(cv::Mat binImg, cv::Mat& outImg, double objWidth, double foclen, int tilesize)
{
	// binImg is small input image from classifer (needs to be 162x91)
	// outImg is large image for viewing and deriving distance (needs to be 2592x1458)
	std::vector<std::vector<cv::Point>> contour_arr;	// store detected contours
	cv::Rect detectedRect;								// define rectange object
	double objDist;										// distance of marker from camera
														//cv::Mat outImView = outImg.clone();

														// Step 1: Find contours in image
	cv::findContours(binImg.clone(), contour_arr, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE); // find all contours in image
	if (contour_arr.size() == 0) return; // check for no detected contours to prevent exception
	std::vector<cv::Rect> detectBB(contour_arr.size());	// vector of detected boundry boxes

														// Step 2: Find Bounds and Draw on Image
	for (int i = 0; i < contour_arr.size(); i++)
	{
		detectBB[i] = cv::boundingRect(contour_arr[i]);
		cv::Rect scaledRect(detectBB[i].x*tilesize, detectBB[i].y*tilesize, detectBB[i].width*tilesize, detectBB[i].height*tilesize); // scale detect rectangle to new resolution for distance calc
		detectBB[i] = scaledRect;
		cv::rectangle(outImg, detectBB[i], cv::Scalar(255, 0, 0), 5, cv::LINE_8, 0); // Draw Bounding Rectangle on Image
		cv::putText(outImg, std::to_string(i + 1), cv::Point(detectBB[i].x + detectBB[i].width, detectBB[i].y), cv::FONT_HERSHEY_SIMPLEX, 5, cv::Scalar(0, 0, 225), 8, 8);
	}
	pyrDown(outImg, outImg); pyrDown(outImg, outImg); //scale image for viewing
	cv::imshow("Im", outImg);
	cv::waitKey(0);
	cv::destroyAllWindows(); // Close All Windows
}


// FUNCTIONS BELOW ARE DEPRECIATED
//// returns distance of object from camera
//double get_objDist(cv::Mat imorig, double objWidth, double foclen)
//{
//	// Declarations
//	cv::Mat imraw = imorig.clone(); // Clone the image to avoid damage on the original 
//	double objDist = 0; // return variable
//	double objPixelWidth = 0; // pixel width of obj in image to be found
//
//	objPixelWidth = detect_rectWidth_contour(imraw); // get width of the rectangle in pixels
//	objDist = objDetectDist(objWidth, objPixelWidth, foclen); // get distance from camera
//
//															  // Close All Windows
//	cv::destroyAllWindows();
//
//	return objDist; // return distance
//}
//
//// detect rectange in image and return the horizontal width using contours
//double detect_rectWidth_contour(cv::Mat im)
//{
//	// Declarations
//	double rectWidth = 0;								// return variable
//	std::vector<std::vector<cv::Point>> contour_arr;	// store detected contours
//	std::vector<cv::Vec4i> hierarch;					// hierarchy array for findContours
//	cv::Mat imdrawContour = im.clone();					// clone of orignal image for drawing contours on
//	cv::Mat imRect = im.clone();						// clone of orignal image for drawing rectangles on
//	cv::Rect detectedRect;								// define rectange object
//	int tilesize = 16;									// tile size of texture classifier (needs to become function argument)
//	resolution origRes = { 2592, 1458 };					// orignal resolution (needs to become function arguement)
//
//															// Step 1: Find contours in image
//	cv::cvtColor(im, im, cv::COLOR_BGR2GRAY);			// Convert BGR to grayscale findContours
//	cv::findContours(im, contour_arr, hierarch, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE); // find all contours in image (alt methods available)
//
//																						   // Step 2: Find Bounds and Draw on Image
//	detectedRect = cv::boundingRect(contour_arr[0]);	// get the bounding rectange dimensions from first contour found
//	cv::resize(imRect, imRect, cv::Size(origRes.width, origRes.height));	// resize image to original size
//	cv::Rect scaledRect(detectedRect.x*tilesize, detectedRect.y*tilesize, detectedRect.width*tilesize, detectedRect.height*tilesize); // scale detect rectangle to new resolution for distance calc
//																																	  // Draw Bounding Rectangle on Image
//	cv::rectangle(imRect, scaledRect, cv::Scalar(0, 0, 255), 3, cv::LINE_8, 0);
//	cv::Mat imRectView = pyrDownDef(imRect, 3);
//	cv::imshow("Bounding Rectangle", imRectView);
//	cv::waitKey(0);
//
//	// Step 2 Alternate Display: Draw All Contours
//	cv::drawContours(imdrawContour, contour_arr, 0, cv::Scalar(0, 0, 255), 3, cv::LINE_8, hierarch);
//	cv::resize(imdrawContour, imdrawContour, cv::Size(origRes.width, origRes.height));	// resize image to original size
//	cv::Mat imContourView = pyrDownDef(imdrawContour, 3);
//	cv::imshow("Detected Contours Drawn On Image", imContourView);
//	cv::waitKey(0);
//	// Draw Multiple Contours Detected
//	//int idx = 0;
//	//for (; idx >= 0; idx = hierarch[idx][0])
//	// cv::drawContours(imdraw, contour_arr, idx, cv::Scalar(0,0,255), 3, 8, hierarch);
//
//	// Close All Windows
//	cv::destroyAllWindows();
//
//	return rectWidth = scaledRect.width; // returned scaled width to match with focal length
//}