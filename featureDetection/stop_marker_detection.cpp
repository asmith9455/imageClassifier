#include <stop_marker_detection.h>

namespace stop_marker_detection
{

struct resolution
{
	int width;
	int height;
};

// returns distance of object from camera
double get_objdist(cv::Mat imorig, double objWidth, double foclen)
{
	// Declarations
	cv::Mat imraw = imorig.clone(); // Clone the image to avoid damage on the original 
	double objDist = 0; // return variable
	double objPixelWidth = 0; // pixel width of obj in image to be found
	// bool markerdetected = false; // to be added later: flag to indicate if marker is visible
	
	objPixelWidth = detect_rectWidth_contour(imraw); // get width of the rectangle in pixels
	objDist = objDetectDist(objWidth, objPixelWidth, foclen); // get distance from camera
	
	// Close All Windows
	cv::destroyAllWindows();

	return objDist; // return distance
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

// detect rectange in image and return the horizontal width using contours
double detect_rectWidth_contour(cv::Mat im) // add int tilesize when ready
{
	// Declarations
	double rectWidth = 0;								// return variable
	std::vector<std::vector<cv::Point>> contour_arr;	// store detected contours
	std::vector<cv::Vec4i> hierarch;					// hierarchy array for findContours
	cv::Mat imdrawContour = im.clone();					// clone of orignal image for drawing contours on
	cv::Mat imRect = im.clone();						// clone of orignal image for drawing rectangles on
	cv::Rect detectedRect;								// define rectange object
	int tilesize = 16;									// tile size of texture classifier (needs to become function argument)
	resolution origRes = {2592, 1458};					// orignal resolution (needs to become function arguement)

	// Step 1: Find contours in image
		cv::cvtColor(im, im, cv::COLOR_BGR2GRAY);			// Convert BGR to grayscale findContours
		cv::findContours(im, contour_arr, hierarch, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE); // find all contours in image
		//cv::findContours(im, contour_arr, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE); // alt methods available, choose depending on process time

	// Step 2: Find Bounds and Draw on Image
		// Find Rectange Width
		imRect = imRect * 255;								// convert binary map to grayscale
		detectedRect = cv::boundingRect(contour_arr[0]);	// get the bounding rectange dimensions from first contour found
		cv::resize(imRect, imRect, cv::Size(origRes.width, origRes.height));	// resize image to original size
		cv::Rect scaledRect(detectedRect.x*tilesize, detectedRect.y*tilesize, detectedRect.width*tilesize, detectedRect.height*tilesize); // scale detect rectangle to new resolution for distance calc
		// Draw Bounding Rectangle on Image
		cv::rectangle(imRect, scaledRect, cv::Scalar(0, 0, 255), 3, cv::LINE_8, 0);
		cv::Mat imRectView = pyrDownDef(imRect, 3);
		cv::imshow("Bounding Rectangle", imRectView);
		cv::waitKey(0);

	// Step 2 Alternate Display: Draw All Contours
		cv::drawContours(imdrawContour, contour_arr, 0, cv::Scalar(0, 0, 255), 3, cv::LINE_8, hierarch);
		cv::resize(imdrawContour, imdrawContour, cv::Size(origRes.width, origRes.height));	// resize image to original size
		cv::Mat imContourView = pyrDownDef(imdrawContour, 3);
		cv::imshow("Detected Contours Drawn On Image", imContourView);
		cv::waitKey(0);
		// Draw Multiple Contours Detected
		//int idx = 0;
		//for (; idx >= 0; idx = hierarch[idx][0])
		// cv::drawContours(imdraw, contour_arr, idx, cv::Scalar(0,0,255), 3, 8, hierarch);
	
	// Close All Windows
		cv::destroyAllWindows();

	return rectWidth = scaledRect.width; // returned scaled width to match with focal length
}

// NOT FUNCTIONAL YET: BACKUP METHOD FOR CONTOUR detect rectange in image and return the horizontal width using simple blob detection
double detect_rectWidth_blob(cv::Mat im)
{
	double rectWidth = 0; // return variable
	cv::Mat imblob;
	std::vector<cv::KeyPoint> k_points;
	cv::SimpleBlobDetector marker_detector;
	marker_detector.detect(im, k_points);
	drawKeypoints(im, k_points, imblob, cv::Scalar(0, 0, 255), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	return rectWidth;
}

// draw a rectangle on the largest contour
void detect_rectangleOnBinaryImage(cv::Mat binImg, cv::Mat& colImg) // add int tilesize when ready
{
    // Declarations						// return variable
    std::vector<std::vector<cv::Point>> contour_arr;	// store detected contours
    std::vector<cv::Vec4i> hierarch;					// hierarchy array for findContours
    //cv::Mat imRect = binImg.clone();						// clone of orignal image for drawing rectangles on
    cv::Rect detectedRect;								// define rectange object
    int tilesize = 16;									// tile size of texture classifier (needs to become function argument)
    //resolution origRes = {2592, 1458};					// orignal resolution (needs to become function arguement)

    // Step 1: Find contours in image
        //cv::cvtColor(im, im, cv::COLOR_BGR2GRAY);			// Convert BGR to grayscale findContours
    try
    {
        cv::findContours(binImg.clone(), contour_arr, hierarch, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE); // find all contours in image
     }
    catch(std::exception& e)
    {
        std::cerr<<"error in findcontours is: "<<e.what()<<std::endl;
    }
        //cv::findContours(im, contour_arr, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE); // alt methods available, choose depending on process time


    if (contour_arr.size() == 0)
        return;

    // Step 2: Find Bounds and Draw on Image
        // Find Rectange Width
        //imRect = imRect * 255;								// convert binary map to grayscale
        detectedRect = cv::boundingRect(contour_arr[0]);	// get the bounding rectange dimensions from first contour found
        //cv::resize(imRect, imRect, cv::Size(origRes.width, origRes.height));	// resize image to original size
        cv::Rect scaledRect(detectedRect.x*tilesize, detectedRect.y*tilesize, detectedRect.width*tilesize, detectedRect.height*tilesize); // scale detect rectangle to new resolution for distance calc
        // Draw Bounding Rectangle on Image

        cv::rectangle(colImg, scaledRect, cv::Scalar(255, 0, 0), 7, cv::LINE_8, 0);
}

}
