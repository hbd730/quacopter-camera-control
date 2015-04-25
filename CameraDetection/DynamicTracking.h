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
	DynamicTracking();
	virtual ~DynamicTracking();
	virtual std::string getName() const;
	virtual void init(cv::Mat& image);
	virtual void setReferenceFrame(cv::Mat& reference);
	virtual bool processFrame(cv::Mat& image);
	virtual void event(Event* event);
	
private:
	CMT m_cmt;
	bool m_initialised;
	cv::Rect m_selection;
};
