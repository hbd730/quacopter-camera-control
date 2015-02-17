//
//  DynamicTracking.h
//  CameraDetection
//
//  Created by Peter Huang on 30/10/2014.
//  Copyright (c) 2014 FlightDynamics. All rights reserved.
//

#ifndef __CameraDetection__DynamicTracking__
#define __CameraDetection__DynamicTracking__

#include "Tracking.h"

class DynamicTracking : public ITracking
{
public:
	DynamicTracking(cv::Point3i& position);
	virtual ~DynamicTracking();
	virtual std::string getName() const;
	virtual void init(cv::Mat& image);
	virtual void setReferenceFrame(cv::Mat& reference);
	virtual bool processFrame(cv::Mat& image);
};

#endif /* defined(__CameraDetection__DynamicTracking__) */
