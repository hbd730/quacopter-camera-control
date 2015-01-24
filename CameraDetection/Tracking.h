//
//  Tracking.h
//  CameraDetection
//
//  Created by Peter Huang on 30/10/2014.
//  Copyright (c) 2014 FlightDynamics. All rights reserved.
//

#ifndef __CameraDetection__ITracking__
#define __CameraDetection__ITracking__

#include <opencv2/opencv.hpp>

// Interface for all tracking algorithm
class ITracking
{
public:
	ITracking(cv::Point3i& position):m_position(position){};
	virtual std::string getName() const = 0;
	virtual cv::Point3i getCurrentPosition() const = 0;
	virtual void init(cv::Mat& image) = 0;
	virtual void setReferenceFrame(cv::Mat& reference) = 0;
	virtual bool processFrame(cv::Mat& image) = 0;
	virtual ~ITracking(){};
protected:
	cv::Point3i m_position;
};

#endif /* defined(__CameraDetection__ITracking__) */
