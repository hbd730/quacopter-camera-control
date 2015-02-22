//
//  TrackingDelegate.h
//  CameraDetection
//
//  Created by Peter Huang on 30/10/2014.
//  Copyright (c) 2014 FlightDynamics. All rights reserved.
//

#pragma once

#include "Tracking.h"
#include "BallTracking.h" 

class TrackingDelegate
{
public:
	enum StrategyType
	{
		kBall,
	 	kStatic,
		kDynamic
	};
	enum StateType
	{
		kInit,
		kProcessFrame
	};
	
	TrackingDelegate();
	virtual ~TrackingDelegate();
	void setStrategy(StrategyType type);
	BallTracking* getBallTracker() const;
	cv::Point3i startTracking(cv::Mat& image);
	cv::Mat getOutputImage() const {return m_tracking->getOutputImage();};
	
private:
	ITracking* m_tracking;
	StateType m_state;
	std::mutex m_mutex;
};
