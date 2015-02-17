//
//  BallTracking.h
//  CameraDetection
//
//  Created by Peter Huang on 30/10/2014.
//  Copyright (c) 2014 FlightDynamics. All rights reserved.
//

#ifndef __CameraDetection__BallTracking__
#define __CameraDetection__BallTracking__

#include "Tracking.h"

class BallTracking : public ITracking
{
public:
	BallTracking(cv::Point3i& position);
	virtual ~BallTracking();
	virtual std::string getName() const;
	virtual void init(cv::Mat& image);
	virtual void setReferenceFrame(cv::Mat& reference);
	virtual bool processFrame(cv::Mat& image);
	void HoughDetection(const cv::Mat& src_gray, const cv::Mat& src_display, int cannyThreshold, int accumulatorThreshold);
	void tackerBarHandler(int pos);
	int calculateDistance(cv::Mat& thresholdImage);
	
private:
	int m_cannyThreshold;
	int m_accumulatorThreshold;
	int gLowH;
	int gHighH;
	int gLowS;
	int gHighS;
	int gLowV;
	int gHighV;
};

#endif /* defined(__CameraDetection__BallTracking__) */
