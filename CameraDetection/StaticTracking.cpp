//
//  StaticTracking.cpp
//  CameraDetection
//
//  Created by Peter Huang on 30/10/2014.
//  Copyright (c) 2014 FlightDynamics. All rights reserved.
//

#include "StaticTracking.h"
using namespace std;
using namespace cv;

// sensitivity value to be used in the absdiff() function
const static int SENSITIVITY_VALUE = 100;
// size of blur used to smooth the intensity image output from absdiff() function
const static int BLUR_SIZE = 10;

StaticTracking::StaticTracking():m_position(0,0),m_boundingRectangle(0,0,0,0)
{
	
}

StaticTracking::~StaticTracking()
{
}

std::string StaticTracking::getName() const
{
	return "Static tracking";
}

void StaticTracking::init(cv::Mat &image)
{

}

void StaticTracking::setReferenceFrame(cv::Mat& reference)
{
	m_referenceFrame = reference;
	cv::cvtColor(m_referenceFrame, m_grayImageRef, COLOR_BGR2GRAY);
}

bool StaticTracking::processFrame(cv::Mat &image)
{
	// make a deep copy of camera current callback image, not a reference
	image.copyTo(m_currentFrame);
	
	// convert current frame to gray scale for frame differencing
	cv::cvtColor(m_currentFrame, m_grayImageCur, COLOR_BGR2GRAY);
	
	// perform frame differencing between current and reference image
	cv::absdiff(m_grayImageRef, m_grayImageCur, m_differenceImage);
	
	// threshold intensity image at a given sensitivity value
	cv::threshold(m_differenceImage, m_thresholdImage, SENSITIVITY_VALUE, 255, THRESH_BINARY);
	
	// blur the image to get rid of the noise. This will output an intensity image
	cv::blur(m_thresholdImage, m_thresholdImage, cv::Size(BLUR_SIZE,BLUR_SIZE));
	
	// threshold again to obtain binary image from blur output
	cv::threshold(m_thresholdImage, m_thresholdImage, SENSITIVITY_VALUE, 255, THRESH_BINARY);
	
	// find the position of moving object and apply as text on image for display
	searchForMovement(m_thresholdImage,image);
	
	// set current frame as reference for the coming frame
	setReferenceFrame(m_currentFrame);
	
	return true;
}

void StaticTracking::searchForMovement(cv::Mat thresholdImage, cv::Mat &cameraFeed)
{
	//these two vectors needed for output of findContours
	vector< vector<cv::Point> > contours;
	vector<cv::Vec4i> hierarchy;
	//find contours of filtered image using openCV findContours function
	findContours(m_thresholdImage, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE); // retrieves external contours
	
	//if contours vector is not empty,object is detected
	if(contours.size() > 0)
	{
		// the largest contour is found at the end of the contours vector
		// this assume that the biggest contour is the object we are looking for.
		vector< vector<cv::Point> > largestContourVec;
		largestContourVec.push_back(contours.at(contours.size()-1));
		
		// make a bounding rectangle around the largest contour then find its centroid
		m_boundingRectangle = cv::boundingRect(largestContourVec.at(0));
		m_position.x = m_boundingRectangle.x + m_boundingRectangle.width/2;
		m_position.y = m_boundingRectangle.y + m_boundingRectangle.height/2;
	}
	// draw circle and cross around the object
	circle(cameraFeed, m_position, 20, Scalar(0,255,0),2);
	line(cameraFeed, m_position, cv::Point(m_position.x,m_position.y-25), Scalar(0,255,0),2);
	line(cameraFeed, m_position, cv::Point(m_position.x,m_position.y+25), Scalar(0,255,0),2);
	line(cameraFeed, m_position, cv::Point(m_position.x-25,m_position.y), Scalar(0,255,0),2);
	line(cameraFeed, m_position, cv::Point(m_position.x+25,m_position.y), Scalar(0,255,0),2);
	
	// apply the position text to the screen
	putText(cameraFeed,"(" + std::to_string(m_position.x)+ "," + std::to_string(m_position.y) + ")", m_position, 4, 1, Scalar(255,0,0),2);
}
