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
	DynamicTracking(cv::Point3i& position);
	virtual ~DynamicTracking();
	virtual std::string getName() const;
	virtual void init(cv::Mat& image);
	virtual void setReferenceFrame(cv::Mat& reference);
	virtual bool processFrame(cv::Mat& image);
	virtual void setSelectedRegion(int x, int y, bool mouseDown);
	
private:
	CMT m_cmt;
	cv::Rect m_selection;
	bool m_initialised;
	cv::Point2f m_initTopLeft;
    cv::Point2f m_initBottomDown;
};
