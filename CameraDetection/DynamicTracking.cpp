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
m_initBottomDown(250,300),
m_initialised(false)
{

}

DynamicTracking::~DynamicTracking()
{
}

void DynamicTracking::setSelectedRegion(int x, int y, bool mouseDown)
{
	if (mouseDown)
	{
		m_selection.x = x;
        m_selection.y = y;

		m_initialised = false;
	}
	else
	{
		m_initTopLeft.x = std::min(x, m_selection.x);
		m_initTopLeft.y = std::min(y, m_selection.y);
		m_initBottomDown.x = std::max(x, m_selection.x);
		m_initBottomDown.y = std::max(y, m_selection.y);
		printf("topLeft is (%f, %f)",m_initTopLeft.x, m_initTopLeft.y);
		printf("BottomDown is (%f, %f)",m_initBottomDown.x, m_initBottomDown.y);
//		m_selection.width = std::abs(m_initTopLeft.x - m_initBottomDown.x);
//		m_selection.height = std::abs(m_initTopLeft.y - m_initBottomDown.y);
		
		m_initialised = true;
	}
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
	static bool proccessNow = false;
	cv::cvtColor(image, m_imageGray, cv::COLOR_RGB2GRAY);
	if(m_initialised)
	{
		m_cmt.initialise(m_imageGray, m_initTopLeft, m_initBottomDown);
		proccessNow = true;
	}
	if(proccessNow)
		m_cmt.processFrame(m_imageGray);
	
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
	
	//imshow("frame", image);

	return true;
}