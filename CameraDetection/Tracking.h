//
//  Tracking.h
//  CameraDetection
//
//  Created by Peter Huang on 30/10/2014.
//  Copyright (c) 2014 FlightDynamics. All rights reserved.
//

#pragma once

#include <opencv2/opencv.hpp>

class Event;

// Interface for all tracking algorithm
class ITracking
{
public:
	virtual std::string getName() const = 0;
	virtual void reset() = 0;
	virtual void event(Event* event) = 0; // event handler
	virtual bool track(cv::Mat& inputImage, cv::Mat& outputImage, cv::Point3i& position) = 0;
	virtual ~ITracking(){};
	
protected:
	ITracking(){};
};
