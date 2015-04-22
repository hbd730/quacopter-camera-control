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
	int calculateDistance(cv::Mat& thresholdImage);
	
private:
	int m_lowH;
	int m_highH;
	int m_lowS;
	int m_highS;
	int m_lowV;
	int m_highV;
};
