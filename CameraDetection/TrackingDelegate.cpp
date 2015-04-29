//
//  TrackingDelegate.cpp
//  CameraDetection
//
//  Created by Peter Huang on 30/10/2014.
//  Copyright (c) 2014 FlightDynamics. All rights reserved.
//

#include "TrackingDelegate.h"
#include "StaticTracking.h"
#include "BallTracking.h"
#include "DynamicTracking.h"
#include "Constant.h"

TrackingDelegate::TrackingDelegate():
	m_tracker(NULL)
{
	addTracker(kBall, new BallTracking());
	addTracker(kStatic, new StaticTracking());
	addTracker(kDynamic, new DynamicTracking());
	setTracker(TrackingDelegate::kBall);
}

TrackingDelegate::~TrackingDelegate()
{
	TrackerList::iterator iter;
	for (iter = m_trackerList.begin(); iter != m_trackerList.end(); iter++)
	{
		delete iter->second;
		iter->second = NULL;
	}
}

void TrackingDelegate::addTracker(TrackerType type, ITracking* tracker)
{
	m_trackerList[type] = tracker;
}

void TrackingDelegate::setTracker(TrackerType type)
{
	TrackerList::iterator iter = m_trackerList.find(type);
	if(iter != m_trackerList.end())
	{
		m_tracker = iter->second;
		m_tracker->reset();
	}
}

bool TrackingDelegate::startTracking(cv::Mat& inputImage, cv::Mat& outputImage, cv::Point3i& foundPos)
{
	return m_tracker->track(inputImage, outputImage, foundPos);
}

void TrackingDelegate::notifyTracker(Event* event)
{
	m_tracker->event(event);
}
