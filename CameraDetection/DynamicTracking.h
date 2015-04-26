//
//  DynamicTracking.h
//  CameraDetection
//
//  Created by Peter Huang on 30/10/2014.
//  Copyright (c) 2014 FlightDynamics. All rights reserved.
//

#pragma once

#include "Tracking.h"
#include "CMT.h"

class DynamicTracking : public ITracking
{
public:
	enum StateType
	{
		kInit,
		kProcessFrame
	};
	DynamicTracking();
	virtual ~DynamicTracking();
	virtual std::string getName() const;
	virtual void reset();
	virtual void event(Event* event);
	virtual bool track(cv::Mat& inputImage, cv::Mat& outputImage, cv::Point3i& position);
	
private:
	CMT m_cmt;
	StateType m_state;
	bool m_clicked;
	cv::Point2f m_topLeft, m_bottomDown;
};
