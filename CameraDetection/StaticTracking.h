//
//  StaticTracking.h
//  CameraDetection
//
//  Created by Peter Huang on 30/10/2014.
//  Copyright (c) 2014 FlightDynamics. All rights reserved.
//

#ifndef __CameraDetection__StaticTracking__
#define __CameraDetection__StaticTracking__

#include "Tracking.h"

class StaticTracking : public ITracking
{
public:
	StaticTracking();
	virtual ~StaticTracking();
	virtual std::string getName() const;
	virtual void init(cv::Mat& image);
	virtual void setReferenceFrame(cv::Mat& reference);
	virtual bool processFrame(cv::Mat& image);
	void searchForMovement(cv::Mat thresholdImage, cv::Mat &cameraFeed);
	
private:
	// the position of the tracking object
	cv::Point m_position;
	
	//bounding rectangle of the object, we will use the center of this as its position.
	cv::Rect m_boundingRectangle;
	
	//the two frames we will be comparing
	cv::Mat m_referenceFrame, m_currentFrame;
	
	//their grayscale images (needed for absdiff() function)
	cv::Mat m_grayImageRef, m_grayImageCur;
	
	//resulting difference image
	cv::Mat m_differenceImage;
	
	//thresholded difference image (for use in findContours() function)
	cv::Mat m_thresholdImage;
};

#endif /* defined(__CameraDetection__StaticTracking__) */
