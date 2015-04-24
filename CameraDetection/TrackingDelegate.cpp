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

bool TrackingDelegate::startTracking(cv::Mat& image, cv::Point3i& foundPos)
{
	m_mutex.lock();
	bool found = false;
	switch(m_state)
	{
		case kInit:
			m_tracking->init(image);
			m_tracking->setReferenceFrame(image); // set the first frame as reference
			m_state = kProcessFrame;
			break;
		case kProcessFrame:
			found = m_tracking->processFrame(image);
			foundPos = m_tracking->getCurrentPosition();
			m_state = kProcessFrame;
			break;
		default:
			break;
	}
	m_mutex.unlock();
	return found;
}

void TrackingDelegate::onParameterChanged(BallTrackerEvent event)
{
	if (BallTracking* ballTracker  = dynamic_cast<BallTracking*>(m_tracking))
	{
		if (event.type() == UIControlEvent::HSVLowGroupChanged)
		{
			switch(event.getID())
			{
				case kFirst:
					ballTracker->setLowH(event.value());
					break;
				case kSecond:
					ballTracker->setLowS(event.value());
					break;
				case kThird:
					ballTracker->setLowV(event.value());
					break;
				default:
					break;
			}
		}
		else if (event.type() == UIControlEvent::HSVHighGroupChanged)
		{
			switch(event.getID())
			{
				case kFirst:
					ballTracker->setHighH(event.value());
					break;
				case kSecond:
					ballTracker->setHighS(event.value());
					break;
				case kThird:
					ballTracker->setHighV(event.value());
					break;
				default:
					break;
			}
		}
	}
}
