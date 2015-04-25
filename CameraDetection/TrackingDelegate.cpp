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
m_tracker(NULL),
m_state(kInit),
m_mutex()
{

}

TrackingDelegate::~TrackingDelegate()
{
	if(m_tracker != NULL)
		delete m_tracker;
}

void TrackingDelegate::setStrategy(StrategyType type)
{
	m_mutex.lock();
	if(m_tracker != NULL)
	{
		delete m_tracker;
		m_tracker = NULL;
		m_state = kInit;
	}
	switch(type)
	{
		case kStatic:
			m_tracker = new StaticTracking();
			break;
		case kDynamic:
			m_tracker = new DynamicTracking();
			break;
		case kBall:
			m_tracker = new BallTracking();
			break;
		default:
			break;
	}
	m_mutex.unlock();
}

bool TrackingDelegate::startTracking(cv::Mat& image, cv::Point3i& foundPos)
{
	m_mutex.lock();
	bool found = false;
	switch(m_state)
	{
		case kInit:
			m_tracker->init(image);
			m_tracker->setReferenceFrame(image); // set the first frame as reference
			m_state = kProcessFrame;
			break;
		case kProcessFrame:
			found = m_tracker->processFrame(image);
			foundPos = m_tracker->getCurrentPosition();
			m_state = kProcessFrame;
			break;
		default:
			break;
	}
	m_mutex.unlock();
	return found;
}

void TrackingDelegate::notifyTracker(Event* event)
{
	m_tracker->event(event);
}
