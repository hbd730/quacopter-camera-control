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
	cv::Point3i startTracking(cv::Mat& image);
	cv::Mat getOutputImage() const {return m_tracking->getOutputImage();};
	
private:
	ITracking* m_tracking;
	StateType m_state;
	std::mutex m_mutex;
};

#endif /* defined(__CameraDetection__TrackingDelegate__) */
