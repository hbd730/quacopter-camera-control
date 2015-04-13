//
//  Tracking.h
//  CameraDetection
//
//  Created by Peter Huang on 30/10/2014.
//  Copyright (c) 2014 FlightDynamics. All rights reserved.
//

#pragma once

#include <opencv2/opencv.hpp>

// Interface for all tracking algorithm
class ITracking
{
public:
	ITracking(cv::Point3i& position):m_position(position),m_outputImage(){};
	cv::Point3i getCurrentPosition() const {return m_position;};
	cv::Mat getOutputImage() const {return m_outputImage;};
	virtual std::string getName() const = 0;
	virtual void init(cv::Mat& image) = 0;
	virtual void setReferenceFrame(cv::Mat& reference) = 0;
	virtual bool processFrame(cv::Mat& image) = 0;
	virtual void setSelectedRegion(int x, int y, bool mouseDown){};
	virtual ~ITracking(){};
protected:
	cv::Point3i m_position;
	cv::Mat m_outputImage;
};
