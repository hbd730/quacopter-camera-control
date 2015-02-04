//
//  BallTracking.cpp
//  CameraDetection
//
//  Created by Peter Huang on 30/10/2014.
//  Copyright (c) 2014 FlightDynamics. All rights reserved.
//

#include "BallTracking.h"

#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace cv;

// initial parameters of interests.
const int kCannyThresholdInitialValue = 200;
const int kAccumulatorThresholdInitialValue = 100;
const int kMaxAccumulatorThreshold = 200;
const int kMaxCannyThreshold = 255;
const std::string kWindowName = "Hough Circle Detection Demo";
const std::string kCannyThresholdTrackbarName = "Canny threshold";
const std::string kAccumulatorThresholdTrackbarName = "Accumulator Threshold";
static int gLowH;
static int gHighH;
static int gLowS;
static int gHighS;
static int gLowV;
static int gHighV;
const float kFoc = 40.0f;
const float kObjectHeight = 35.0f;


BallTracking::BallTracking(cv::Point3i& position):ITracking(position),
m_cannyThreshold(kCannyThresholdInitialValue),
m_accumulatorThreshold(kAccumulatorThresholdInitialValue)
{
	gLowH = 5;
	gHighH = 23;
	gLowS = 27;
	gHighS = 218;
	gLowV = 154;
	gHighV = 255;
}

BallTracking::~BallTracking()
{
}

std::string BallTracking::getName() const
{
	return "Ball tracking";
}

cv::Point3i BallTracking::getCurrentPosition() const
{
	return m_position;
}

static void on_trackbar( int pos, void* value) //this callback print the current value when slider bar changes
{
	printf("LowH is : %d\n", gLowH);
	printf("HighH is : %d\n", gHighH);
	printf("LowS is : %d\n", gLowS);
	printf("HighS is : %d\n", gHighS);
	printf("LowV is : %d\n", gLowV);
	printf("HighV is : %d\n", gHighV);
}

void BallTracking::init(cv::Mat &image)
{
	namedWindow( kWindowName, WINDOW_NORMAL |WINDOW_OPENGL );
	resizeWindow(kWindowName, 400, 400);
	cv::createTrackbar(kCannyThresholdTrackbarName, kWindowName, &m_cannyThreshold, kMaxCannyThreshold);
	cv::createTrackbar(kAccumulatorThresholdTrackbarName, kWindowName, &m_accumulatorThreshold, kMaxAccumulatorThreshold);
	//Create trackbars in "Control" window
	cv::createTrackbar("LowH", kWindowName, &gLowH, 179, on_trackbar); //Hue (0 - 179)
	cv::createTrackbar("HighH", kWindowName, &gHighH, 179, on_trackbar);
	
	cv::createTrackbar("LowS", kWindowName, &gLowS, 255, on_trackbar); //Saturation (0 - 255)
	cv::createTrackbar("HighS", kWindowName, &gHighS, 255, on_trackbar);
	
	cv::createTrackbar("LowV", kWindowName, &gLowV, 255, on_trackbar);//Value (0 - 255)
	cv::createTrackbar("HighV", kWindowName, &gHighV, 255, on_trackbar);
}

void BallTracking::setReferenceFrame(cv::Mat& reference)
{
	
}

bool BallTracking::processFrame(cv::Mat &image)
{
	// Convert it to gray, Hough detection works well only on gray image
	cvtColor(image, m_grayImageCur, COLOR_BGR2GRAY);
	
	// Reduce the noise so we avoid false circle detection
	GaussianBlur(m_grayImageCur, m_grayImageCur, Size(9, 9), 2, 2);

	// those paramaters cannot be =0
	// so we must check here
	m_cannyThreshold = std::max(m_cannyThreshold, 1);
	m_accumulatorThreshold = std::max(m_accumulatorThreshold, 1);

	Mat imgHSV;
	cvtColor(image, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV
	
	Mat imgThresholded;
	inRange(imgHSV, Scalar(gLowH, gLowS, gLowV), Scalar(gHighH, gHighS, gHighV), imgThresholded); //Threshold the image
	
	// morphological opening (removes small objects from the foreground)
	erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
	dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
	
	// morphological closing (removes small holes from the foreground)
	dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
	erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
	
	imshow(kWindowName, imgThresholded);
	
	// Calculate the moments of the thresholded image
	Moments oMoments = moments(imgThresholded);
	
	double dM01 = oMoments.m01;
	double dM10 = oMoments.m10;
	double dArea = oMoments.m00;
	if (dArea > 10000)  // we consider there is an object if area larger than 10000
	{
		// calculate the position of the ball
		int posX = dM10 / dArea;
		int posY = dM01 / dArea;
		
		if (posX >= 0 && posY >= 0)
		{
			m_position.x = posX;
			m_position.y = posY;
				
			// To-do need to think about how to combine shape and color detection properly.
			//HoughDetection(m_grayImageCur, image, m_cannyThreshold, m_accumulatorThreshold);
			
			m_position.z = calculateDistance(imgThresholded);
			Point center(posX, posY);
			// circle center
			circle(image, center, 3, Scalar(0,255,0), -1, 8, 0);
						
			// Put position text around
			putText(image,"(" + std::to_string(m_position.x)+ "," + std::to_string(m_position.y) \
					+ "," + std::to_string(m_position.z) + ")", center, 4, 1, Scalar(255,0,0),2);
		}
	}
	return true;
}

int BallTracking::calculateDistance(Mat& thresholdImage)
{
	static int distance = 0;
	//these two vectors needed for output of findContours
	vector< vector<cv::Point> > contours;
	vector<cv::Vec4i> hierarchy;
	//find contours of filtered image using openCV findContours function
	findContours(thresholdImage, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE); // retrieves external contours

	//if contours vector is not empty,object is detected
	if(contours.size() > 0)
	{
		cv::Rect boundingRectangle;
		// the largest contour is found at the end of the contours vector
		vector< vector<cv::Point> > largestContourVec;
		largestContourVec.push_back(contours.at(contours.size()-1));
		
		// make a bounding rectangle around the largest contour then find its centroid
		boundingRectangle = cv::boundingRect(largestContourVec.at(0));
		int objectImageHeight = boundingRectangle.height;
		if(objectImageHeight > 0)
		{
			int temp = (kFoc * kObjectHeight / (static_cast<float>(objectImageHeight) * 0.227f));
			if(temp < 350)
				distance = temp;
		}
	}
	return distance;
}

static double euclideanDist(Point p, Point q)
{
	Point diff = p - q;
	return cv::sqrt(diff.x*diff.x + diff.y*diff.y);
}

void BallTracking::HoughDetection(const Mat& src_gray, const Mat& src_display, int cannyThreshold, int accumulatorThreshold)
{
	// hold all detected circles
	std::vector<Vec3f> circles;
	Point colorObjectCenter(m_position.x,m_position.y);
	
	HoughCircles(src_gray, circles, HOUGH_GRADIENT, 1, src_gray.rows/4, cannyThreshold, accumulatorThreshold, 0, 0);
	
	// here is ugly, create the distance array, holding all distance value between color detected center and shape detected center
	std::vector<double> distanceArray;
	for( size_t i = 0; i < circles.size(); i++ )
	{
		Point circleCenter(cvRound(circles[i][0]), cvRound(circles[i][1]));
		double distance = euclideanDist(circleCenter, colorObjectCenter);
		distanceArray.push_back(distance);
	}
	
	// if pingpong is the unique object in the background we filter out other possible orange objects in the background
	if(circles.size() > 0)
	{
		std::vector<double>::iterator it = min_element(distanceArray.begin(), distanceArray.end());
		int index = (int)distance(distanceArray.begin(),it);
		colorObjectCenter.x = cvRound(circles[index][0]);
		colorObjectCenter.y = cvRound(circles[index][1]);
		int radius = cvRound(circles[index][2]);
		circle(src_display, colorObjectCenter, radius, Scalar(0,0,255), 3, 8, 0);
	}
}




