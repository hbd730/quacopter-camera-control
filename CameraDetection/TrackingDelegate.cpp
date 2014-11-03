//
//  TrackingDelegate.cpp
//  CameraDetection
//
//  Created by Peter Huang on 30/10/2014.
//  Copyright (c) 2014 FlightDynamics. All rights reserved.
//

#include "TrackingDelegate.h"
#include "StaticTracking.h"
#include "DynamicTracking.h"
#include "BallTracking.h"

TrackingDelegate::TrackingDelegate():
m_tracking(NULL),
m_state(kInit),
m_mutex()
{

}

TrackingDelegate::~TrackingDelegate()
{
	if(m_tracking != NULL)
		delete m_tracking;
}

void TrackingDelegate::setStrategy(StrategyType type)
{
	m_mutex.lock();
	if(m_tracking != NULL)
	{
		delete m_tracking;
		m_tracking = NULL;
		m_state = kInit;
	}
	switch(type)
	{
		case kStatic:
			m_tracking = new StaticTracking();
			break;
		case kDynamic:
			m_tracking = new DynamicTracking();
			break;
		case kBall:
			m_tracking = new BallTracking();
			break;
		default:
			break;
	}
	m_mutex.unlock();
}

void TrackingDelegate::startTracking(cv::Mat& currentFrame)
{
	m_mutex.lock();
	switch(m_state)
	{
		case kInit:
			m_tracking->init(currentFrame);
			m_tracking->setReferenceFrame(currentFrame); // set the first frame as reference
			m_state = kProcessFrame;
			break;
		case kProcessFrame:
			m_tracking->processFrame(currentFrame);
			m_state = kProcessFrame;
			break;
		default:
			break;
	}
	m_mutex.unlock();
}

