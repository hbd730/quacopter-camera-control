//
//  TrackingDelegate.h
//  CameraDetection
//
//  Created by Peter Huang on 30/10/2014.
//  Copyright (c) 2014 FlightDynamics. All rights reserved.
//

#ifndef __CameraDetection__TrackingDelegate__
#define __CameraDetection__TrackingDelegate__

#include "Tracking.h"

class TrackingDelegate
{
public:
	enum StrategyType
	{
		kStatic,
		kDynamic,
		kBall
	};
	enum StateType
	{
		kInit,
		kProcessFrame
	};
	
	TrackingDelegate();
	virtual ~TrackingDelegate();
	void setStrategy(StrategyType type);
	void startTracking(cv::Mat& image);
	
private:
	ITracking* m_tracking;
	uint32_t m_state;
	std::mutex m_mutex;
};

#endif /* defined(__CameraDetection__TrackingDelegate__) */
