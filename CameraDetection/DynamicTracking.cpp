//
//  DynamicTracking.cpp
//  CameraDetection
//
//  Created by Peter Huang on 30/10/2014.
//  Copyright (c) 2014 FlightDynamics. All rights reserved.
//

#include "DynamicTracking.h"


DynamicTracking::DynamicTracking(cv::Point3i& position):ITracking(position)
{

}

DynamicTracking::~DynamicTracking()
{
}

std::string DynamicTracking::getName() const
{
	return "Dynamic tracking";
}

cv::Point3i DynamicTracking::getCurrentPosition() const
{
	return m_position;
}

void DynamicTracking::init(cv::Mat &image)
{
	
}

void DynamicTracking::setReferenceFrame(cv::Mat& reference)
{

}

bool DynamicTracking::processFrame(cv::Mat &image)
{
	
	return true;
}