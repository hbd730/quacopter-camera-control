//
//  StaticTracking.h
//  CameraDetection
//
//  Created by Peter Huang on 30/10/2014.
//  Copyright (c) 2014 FlightDynamics. All rights reserved.
//

#pragma once

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
	cv::Mat m_currentFrame;
	cv::Mat m_grayImageRef;
	cv::Mat m_thresholdImage;
};
