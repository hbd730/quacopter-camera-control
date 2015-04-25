//
//  DynamicTracking.cpp
//  CameraDetection
//
//  Created by Peter Huang on 30/10/2014.
//  Copyright (c) 2014 FlightDynamics. All rights reserved.
//

#include "DynamicTracking.h"
#include "Event.h"

DynamicTracking::DynamicTracking():
ITracking(),
m_initialised(false)
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
	m_outputImage = cv::Mat::zeros(image.rows,image.cols, image.type());
}

void DynamicTracking::setReferenceFrame(cv::Mat& reference)
{
	// not applicable
}

bool DynamicTracking::processFrame(cv::Mat &image)
{
	m_outputImage = image; // flush previous content with new image
	cv::cvtColor(image, m_outputImage, cv::COLOR_RGB2GRAY); // m_outputImage is a gray image here
	if (m_initialised)
	{
		m_cmt.processFrame(m_outputImage);
		
		for(int i = 0; i < m_cmt.trackedKeypoints.size(); i++)
			cv::circle(image, m_cmt.trackedKeypoints[i].first.pt, 3, cv::Scalar(255,255,255));
		
		cv::line(image, m_cmt.topLeft, m_cmt.topRight, cv::Scalar(0,0,255),2);
		cv::line(image, m_cmt.topRight, m_cmt.bottomRight, cv::Scalar(0,0,255),2);
		cv::line(image, m_cmt.bottomRight, m_cmt.bottomLeft, cv::Scalar(0,0,255),2);
		cv::line(image, m_cmt.bottomLeft, m_cmt.topLeft, cv::Scalar(0,0,255),2);
	}
	
	// Todo update position
	return true;
}

void DynamicTracking::event(Event* event)
{
	if (event->type() == Event::MousePressedOnView)
	{
		MouseEvent* e = static_cast<MouseEvent*>(event);
		m_selection.x = e->getX();
		m_selection.y = e->getY();
		m_initialised = false;
	}
	else if (event->type() == Event::MouseReleasedOnview)
	{
		MouseEvent* e = static_cast<MouseEvent*>(event);
		cv::Point2f topLeft;
		cv::Point2f bottomDown;
		topLeft.x = std::min(e->getX(), m_selection.x);
		topLeft.y = std::min(e->getY(), m_selection.y);
		bottomDown.x = std::max(e->getX(), m_selection.x);
		bottomDown.y = std::max(e->getY(), m_selection.y);
#ifdef DEBUG_MODE
		printf("topLeft is (%f, %f)\n",topLeft.x, topLeft.y);
		printf("BottomDown is (%f, %f)\n",bottomDown.x, bottomDown.y);
#endif
		if (m_cmt.initialise(m_outputImage, topLeft, bottomDown) == 0)
			m_initialised = true;
	}
}
