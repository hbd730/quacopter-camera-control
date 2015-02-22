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
	enum HSVMetrics  // orange color hsv space 
	{
		kLowH = 5,
		kHighH = 23,
		kLowS = 27,
		kHighS = 218,
		kLowV = 154,
		kHighV = 255
	};
	BallTracking(cv::Point3i& position);
	virtual ~BallTracking();
	virtual std::string getName() const;
	virtual void init(cv::Mat& image);
	virtual void setReferenceFrame(cv::Mat& reference);
	virtual bool processFrame(cv::Mat& image);
	void HoughDetection(const cv::Mat& src_gray, const cv::Mat& src_display, int cannyThreshold, int accumulatorThreshold);
	void tackerBarHandler(int pos);
	int calculateDistance(cv::Mat& thresholdImage);
	void setLowH(int value){ m_lowH = value; printf("LowH is : %d\n", m_lowH);}
	void setHighH(int value){ m_highH = value; printf("HighH is : %d\n", m_highH);}
	void setLowS(int value){ m_lowS = value; printf("LowS is : %d\n", m_lowS);}
	void setHighS(int value){ m_highS = value; printf("HighS is : %d\n", m_highS);}
	void setLowV(int value){ m_lowV = value; printf("LowV is : %d\n", m_lowV);}
	void setHighV(int value){ m_highV = value; printf("HighV is : %d\n", m_highV);}
	
private:
	int m_cannyThreshold;
	int m_accumulatorThreshold;
	int m_lowH;
	int m_highH;
	int m_lowS;
	int m_highS;
	int m_lowV;
	int m_highV;
};
