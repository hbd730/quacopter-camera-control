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
const int kAccumulatorThresholdInitialValue = 50;
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

BallTracking::BallTracking():m_position(0,0),
m_cannyThreshold(kCannyThresholdInitialValue),
m_accumulatorThreshold(kAccumulatorThresholdInitialValue)
{
	gLowH = 2;
	gHighH = 54;
	gLowS = 16;
	gHighS = 247;
	gLowV = 150;
	gHighV = 255;
}

BallTracking::~BallTracking()
{
}

std::string BallTracking::getName() const
{
	return "Ball tracking";
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
	namedWindow( kWindowName, WINDOW_AUTOSIZE );
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
	erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
	dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
	
	// morphological closing (removes small holes from the foreground)
	dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
	erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
	
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
			//Draw a red line from the previous point to the current point
			m_position.x = posX;
			m_position.y = posY;
			
			// To-do need to think about how to combine shape and color detection properly.
			// Hough transform to detect circle does not work welll when object moving rate is larger than fram rate.
			HoughDetection(m_grayImageCur, image, m_cannyThreshold, m_accumulatorThreshold);
			
			// circle center
			circle(image, m_position, 3, Scalar(0,255,0), -1, 8, 0);
			
			// put position text around
			putText(image,"(" + std::to_string(m_position.x)+ "," + std::to_string(m_position.y) + ")", m_position, 4, 1, Scalar(255,0,0),2);
		}
	}
	return true;
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
	
	HoughCircles(src_gray, circles, HOUGH_GRADIENT, 1, src_gray.rows/8, cannyThreshold, accumulatorThreshold, 0, 0);
	
	// here is ugly, create the distance array, holding all distance value between color detected center and shape detected center
	std::vector<double> distanceArray;
	for( size_t i = 0; i < circles.size(); i++ )
	{
		Point circleCenter(cvRound(circles[i][0]), cvRound(circles[i][1]));
		double distance = euclideanDist(circleCenter, m_position);
		distanceArray.push_back(distance);
		
		// mark other detected round objects on the original image to debug parameters
		int radius = cvRound(circles[i][2]);
		circle(src_display, circleCenter, radius, Scalar(0,0,255), 3, 8, 0);
	}
	
	// if pingpong is the unique object in the background we filter out other possible orange objects in the background
	if(circles.size() == 1)
	{
		std::vector<double>::iterator it = min_element(distanceArray.begin(), distanceArray.end());
		int index = (int)distance(distanceArray.begin(),it);
		m_position.x = cvRound(circles[index][0]);
		m_position.y = cvRound(circles[index][1]);
	}
}




