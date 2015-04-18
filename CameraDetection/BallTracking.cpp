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

// Camera and tracking object information.
const float kFoc = 40.0f;
const float kObjectHeight = 35.0f;

BallTracking::BallTracking():
ITracking(),
m_lowH(kLowH),m_highH(kHighH),m_lowS(kLowS),m_highS(kHighS),m_lowV(kLowV),m_highV(kHighV)
{
}

BallTracking::~BallTracking()
{
}

std::string BallTracking::getName() const
{
	return "Ball tracking";
}

void BallTracking::init(cv::Mat &image)
{
	m_outputImage = cv::Mat::zeros(image.rows,image.cols, image.type());
}

void BallTracking::setReferenceFrame(cv::Mat& reference)
{
	// not applicable
}

bool BallTracking::processFrame(cv::Mat &image)
{
#ifdef CIRCLE_DETECTION
	int cannyThreshold;
	int accumulatorThreshold;
	const int kCannyThresholdInitialValue = 200;
	const int kAccumulatorThresholdInitialValue = 100;
	// Convert it to gray, Hough detection works well only on gray image
	cvtColor(image, m_outputImage, COLOR_BGR2GRAY);
	// Reduce the noise so we avoid false circle detection
	GaussianBlur(m_outputImage, m_outputImage, Size(9, 9), 2, 2);

	// these paramaters cannot be =0
	cannyThreshold = std::max(kCannyThresholdInitialValue, 1);
	accumulatorThreshold = std::max(kAccumulatorThresholdInitialValue, 1);
#endif
	
	Mat imgHSV;
	cvtColor(image, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV
	
	inRange(imgHSV, Scalar(m_lowH, m_lowS, m_lowV), Scalar(m_highH, m_highS, m_highV), m_outputImage); //Threshold the image
	
	// morphological opening (removes small objects from the foreground)
	erode(m_outputImage, m_outputImage, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
	dilate(m_outputImage, m_outputImage, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
	
	// morphological closing (removes small holes from the foreground)
	dilate(m_outputImage, m_outputImage, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
	erode(m_outputImage, m_outputImage, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
	
	// Calculate the moments of the thresholded image
	Moments oMoments = moments(m_outputImage);
	
	double dM01 = oMoments.m01;
	double dM10 = oMoments.m10;
	double dArea = oMoments.m00;
	if (dArea > 10000)  // we consider there is an object if area larger than 10000
	{
		// calculaste the position of the ball
		int posX = dM10 / dArea;
		int posY = dM01 / dArea;
		
		if (posX >= 0 && posY >= 0)
		{
			m_position.x = posX;
			m_position.y = posY;
				
			// To-do need to think about how to combine shape and color detection properly.
#ifdef CIRCLE_DETECTION
			HoughDetection(m_outputImage, image, cannyThreshold, accumulatorThreshold);
#endif
			m_position.z = calculateDistance(m_outputImage);
			Point center(posX, posY);
			// circle center
			circle(image, center, 4, Scalar(0,255,0), -1, 8, 0);
						
			// Put position text around
			putText(image,"(" + std::to_string(m_position.x)+ "," + std::to_string(m_position.y) \
					+ "," + std::to_string(m_position.z) + ")", center, 4, 2, Scalar(255,0,0), 2);
		}
		return true;
	}
	else
		return false;
}

int BallTracking::calculateDistance(Mat& thresholdImage)
{
	static int distance = 0;
	//these two vectors needed for output of findContours
	vector< vector<cv::Point> > contours;
	vector<cv::Vec4i> hierarchy;
	Mat thImgae;
	thresholdImage.copyTo(thImgae);
	//find contours of filtered image using openCV findContours function, it modifys the input image
	findContours(thImgae, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE); 

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
			if(temp < 550)
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




