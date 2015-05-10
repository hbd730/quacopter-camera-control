//
//  StaticTracking.h
//  CameraDetection
//
//  Created by Peter Huang on 30/10/2014.
//  Copyright (c) 2014 Peter Huang. All rights reserved.
//

#pragma once

#include "Tracking.h"

class StaticTracking : public ITracking
{
public:
	enum StateType
	{
		kInit,
		kProcessFrame
	};
	StaticTracking();
	virtual ~StaticTracking();
	virtual std::string getName() const;
	virtual void reset();
	virtual void event(Event* event);
	virtual bool track(cv::Mat& inputImage, cv::Mat& outputImage, cv::Point3i& position);

protected:
	void setReferenceFrame(cv::Mat& reference);
	bool processFrame(cv::Mat& inputImage, cv::Mat& outputImage, cv::Point3i& position);
	void searchForMovement(cv::Mat thresholdImage, cv::Mat &cameraFeed);
	
private:
	StateType m_state;
	cv::Mat m_currentFrame;
	cv::Mat m_grayImageRef;
	cv::Mat m_thresholdImage;
};
