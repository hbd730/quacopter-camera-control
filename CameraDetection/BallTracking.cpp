//
//  BallTracking.cpp
//  CameraDetection
//
//  Created by Peter Huang on 30/10/2014.
//  Copyright (c) 2014 FlightDynamics. All rights reserved.
//

#include "BallTracking.h"
#include "Constant.h"
#include "Event.h"

using namespace std;
using namespace cv;

// Camera and tracking object information.
const static float kFoc = 40.0f;
const static float kObjectHeight = 35.0f;

static int calculateDistance(Mat& thresholdImage)
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

void BallTracking::reset()
{
	// not applicable
}

bool BallTracking::track(cv::Mat& inputImage, cv::Mat& outputImage, cv::Point3i& position)
{
	Mat imgHSV;
	cvtColor(inputImage, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV
	
	inRange(imgHSV, Scalar(m_lowH, m_lowS, m_lowV), Scalar(m_highH, m_highS, m_highV), outputImage); //Threshold the image
	
	// morphological opening (removes small objects from the foreground)
	erode(outputImage, outputImage, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
	dilate(outputImage, outputImage, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
	
	// morphological closing (removes small holes from the foreground)
	dilate(outputImage, outputImage, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
	erode(outputImage, outputImage, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
	
	// Calculate the moments of the thresholded image
	Moments oMoments = moments(outputImage);
	
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
			position.x = posX;
			position.y = posY;
			position.z = calculateDistance(outputImage);
			
			Point center(posX, posY);
			// circle center
			circle(inputImage, center, 4, Scalar(0,255,0), -1, 8, 0);
						
			// Put position text around
			putText(inputImage,"(" + std::to_string(position.x)+ "," + std::to_string(position.y) \
					+ "," + std::to_string(position.z) + ")", center, 4, 2, Scalar(255,0,0), 2);
		}
		return true;
	}
	else
		return false;
}

void BallTracking::event(Event* event)
{
	if (event->type() == Event::HSVLowGroupChanged)
	{
		HSVControlEvent* e = static_cast<HSVControlEvent*>(event);
		switch(e->getID())
		{
			case kFirst:
				m_lowH = e->value();
				break;
			case kSecond:
				m_lowS = e->value();
				break;
			case kThird:
				m_lowV = e->value();
				break;
			default:
				break;
		}
	}
	else if (event->type() == Event::HSVHighGroupChanged)
	{
		HSVControlEvent* e = static_cast<HSVControlEvent*>(event);
		switch(e->getID())
		{
			case kFirst:
				m_highH = e->value();
				break;
			case kSecond:
				m_highS = e->value();
				break;
			case kThird:
				m_highV = e->value();
				break;
			default:
				break;
		}
	}
}
