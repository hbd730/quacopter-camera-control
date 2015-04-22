//
//  TrackingDelegate.h
//  CameraDetection
//
//  Created by Peter Huang on 30/10/2014.
//  Copyright (c) 2014 FlightDynamics. All rights reserved.
//

#pragma once

#include "Tracking.h"

class UIControlEvent
{
public:
	enum EventType
	{
		HSVLowGroupChanged,
		HSVHighGroupChanged
	};
	UIControlEvent(EventType type):m_type(type){};
	virtual ~UIControlEvent(){};
	EventType type() const { return m_type; };
protected:
	EventType m_type;
};

class BallTrackerEvent: public UIControlEvent
{
public:
	BallTrackerEvent(EventType type, int parameterID, int value)
		:UIControlEvent(type),m_id(parameterID),m_value(value){};
	~BallTrackerEvent(){};
	int getID() const { return m_id; };
	int value() const { return m_value; };
private:
	int m_id;
	int m_value;
};

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
	void onParameterChanged(BallTrackerEvent event);
	ITracking* getCurrentTracker() const { return m_tracking; };
	cv::Mat getOutputImage() const { return m_tracking->getOutputImage(); };
	
private:
	ITracking* m_tracking;
	StateType m_state;
	std::mutex m_mutex;
};
