//
//  BallTracking.h
//  CameraDetection
//
//  Created by Peter Huang on 30/10/2014.
//  Copyright (c) 2014 FlightDynamics. All rights reserved.
//

#pragma once

#include "Tracking.h"

class BallTracking : public ITracking
{
public:
	enum HSVType
	{
		kH,
		kS,
		kV
	};
	BallTracking();
	virtual ~BallTracking();
	virtual std::string getName() const;
	virtual void init(cv::Mat& image);
	virtual void setReferenceFrame(cv::Mat& reference);
	virtual bool processFrame(cv::Mat& image);
	virtual void event(Event* event);
private:
	int m_lowH;
	int m_highH;
	int m_lowS;
	int m_highS;
	int m_lowV;
	int m_highV;
};
