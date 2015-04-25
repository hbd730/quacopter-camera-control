//
//  TrackingDelegate.h
//  CameraDetection
//
//  Created by Peter Huang on 30/10/2014.
//  Copyright (c) 2014 FlightDynamics. All rights reserved.
//

#pragma once

#include "Tracking.h"

class Event;

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
	bool startTracking(cv::Mat& image, cv::Point3i& foundPos);
	void notifyTracker(Event* event);
	cv::Mat getOutputImage() const { return m_tracker->getOutputImage(); };
	
private:
	ITracking* m_tracker;
	StateType m_state;
	std::mutex m_mutex;
};
