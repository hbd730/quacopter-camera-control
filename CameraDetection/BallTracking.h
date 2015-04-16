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
	// Online caculator use range H = 0- 360, S = 0-100, V = 0-100, In OpenCV, H = 0-180, S = 0-255, V = 0-255
	// Define light green Color
	enum HSVMetrics
	{
		kLowH = 20,
		kHighH = 69,
		kLowS = 39,
		kHighS = 132,
		kLowV = 182,
		kHighV = 255
	};
	
	BallTracking();
	virtual ~BallTracking();
	virtual std::string getName() const;
	virtual void init(cv::Mat& image);
	virtual void setReferenceFrame(cv::Mat& reference);
	virtual bool processFrame(cv::Mat& image);
	void setLowH(int value){ m_lowH = value;}
	void setHighH(int value){ m_highH = value;}
	void setLowS(int value){ m_lowS = value;}
	void setHighS(int value){ m_highS = value;}
	void setLowV(int value){ m_lowV = value;}
	void setHighV(int value){ m_highV = value;}
	
protected:
	void HoughDetection(const cv::Mat& src_gray, const cv::Mat& src_display, int cannyThreshold, int accumulatorThreshold);
	void tackerBarHandler(int pos);
	int calculateDistance(cv::Mat& thresholdImage);
	
private:
	int m_lowH;
	int m_highH;
	int m_lowS;
	int m_highS;
	int m_lowV;
	int m_highV;
};
