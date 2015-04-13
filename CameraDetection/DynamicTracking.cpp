//
//  DynamicTracking.cpp
//  CameraDetection
//
//  Created by Peter Huang on 30/10/2014.
//  Copyright (c) 2014 FlightDynamics. All rights reserved.
//

#include "DynamicTracking.h"

DynamicTracking::DynamicTracking(cv::Point3i& position)
:ITracking(position),
m_initTopLeft(0,0),
m_initBottomDown(150,200)
{

}

DynamicTracking::~DynamicTracking()
{
}

std::string DynamicTracking::getName() const
{
	return "Dynamic tracking";
}

void DynamicTracking::init(cv::Mat &image)
{
	image.copyTo(m_outputImage);
}

void DynamicTracking::setReferenceFrame(cv::Mat& reference)
{

}

bool DynamicTracking::processFrame(cv::Mat &image)
{
	cv::Mat im_gray;
	cv::cvtColor(image, im_gray, cv::COLOR_RGB2GRAY);
	static bool initialised = true;
	if(initialised)
	{
		m_cmt.initialise(im_gray, m_initTopLeft, m_initBottomDown);
		initialised = false;
	}
	m_cmt.processFrame(im_gray);
	
	for(int i = 0; i < m_cmt.trackedKeypoints.size(); i++)
		cv::circle(image, m_cmt.trackedKeypoints[i].first.pt, 3, cv::Scalar(255,255,255));
	
	cv::line(image, m_cmt.topLeft, m_cmt.topRight, cv::Scalar(255,255,255));
	cv::line(image, m_cmt.topRight, m_cmt.bottomRight, cv::Scalar(255,255,255));
	cv::line(image, m_cmt.bottomRight, m_cmt.bottomLeft, cv::Scalar(255,255,255));
	cv::line(image, m_cmt.bottomLeft, m_cmt.topLeft, cv::Scalar(255,255,255));
	
#ifdef DEBUG_MODE
	qDebug() << "trackedKeypoints";
	for(int i = 0; i<cmt.trackedKeypoints.size(); i++)
		qDebug() << cmt.trackedKeypoints[i].first.pt.x << cmt.trackedKeypoints[i].first.pt.x << cmt.trackedKeypoints[i].second;
	qDebug() << "box";
	qDebug() << cmt.topLeft.x << cmt.topLeft.y;
	qDebug() << cmt.topRight.x << cmt.topRight.y;
	qDebug() << cmt.bottomRight.x << cmt.bottomRight.y;
	qDebug() << cmt.bottomLeft.x << cmt.bottomLeft.y;
#endif
	
	imshow("frame", image);
	//cv::waitKey(1);

	return true;
}