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
m_initTopLeft(320,180),
m_initBottomDown(960,540),
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
		printf("cliked point is (%d, %d)\n", x, y);

		//m_initialised = false;
	}
	else
	{
		m_initTopLeft.x = std::min(x, m_selection.x);
		m_initTopLeft.y = std::min(y, m_selection.y);
		m_initBottomDown.x = std::max(x, m_selection.x);
		m_initBottomDown.y = std::max(y, m_selection.y);
		printf("topLeft is (%f, %f)\n",m_initTopLeft.x, m_initTopLeft.y);
		printf("BottomDown is (%f, %f)\n",m_initBottomDown.x, m_initBottomDown.y);
		m_initTopLeft.x = 520;
		m_initTopLeft.y = 280;
		m_initBottomDown.x = 760;
		m_initBottomDown.y = 440;
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
	cv::Mat m_imageGray;
	cv::cvtColor(image, m_imageGray, cv::COLOR_RGB2GRAY);
	if(m_initialised)
	{
		m_cmt.initialise(m_imageGray, m_initTopLeft, m_initBottomDown);
		proccessNow = true;
		m_initialised = false;
	}
	if(proccessNow)
		m_cmt.processFrame(m_imageGray);
	
	for(int i = 0; i < m_cmt.trackedKeypoints.size(); i++)
		cv::circle(image, m_cmt.trackedKeypoints[i].first.pt, 3, cv::Scalar(255,255,255));
	
	cv::line(image, m_cmt.topLeft, m_cmt.topRight, cv::Scalar(0,0,255),2);
	cv::line(image, m_cmt.topRight, m_cmt.bottomRight, cv::Scalar(0,0,255),2);
	cv::line(image, m_cmt.bottomRight, m_cmt.bottomLeft, cv::Scalar(0,0,255),2);
	cv::line(image, m_cmt.bottomLeft, m_cmt.topLeft, cv::Scalar(0,0,255),2);
	
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