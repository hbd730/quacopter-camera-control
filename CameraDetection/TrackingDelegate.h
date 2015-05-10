//
//  TrackingDelegate.h
//  CameraDetection
//
//  Created by Peter Huang on 30/10/2014.
//  Copyright (c) 2014 Peter Huang. All rights reserved.
//

#pragma once

#include "Tracking.h"

class Event;

class TrackingDelegate
{
public:
	enum TrackerType
	{
		kBall,
	 	kStatic,
		kDynamic
	};
	
	TrackingDelegate();
	virtual ~TrackingDelegate();
	std::string getName(TrackerType type);
	void setTracker(TrackerType type);
	void notifyTracker(Event* event);
	bool startTracking(cv::Mat& inputImage, cv::Mat& outputImage, cv::Point3i& foundPos);
	
private:
	ITracking* m_tracker;
	typedef std::map<TrackerType, ITracking*> TrackerList;
	TrackerList m_trackerList;
	void addTracker(TrackerType type, ITracking* tracker);
};
