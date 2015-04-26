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
m_state(kInit)
{
}

DynamicTracking::~DynamicTracking()
{
}

std::string DynamicTracking::getName() const
{
	return "Dynamic tracking";
}

void DynamicTracking::reset()
{
	m_state = kInit;
}

bool DynamicTracking::track(cv::Mat& inputImage, cv::Mat& outputImage, cv::Point3i& position)
{
	// flush previous content with new image
	outputImage = inputImage;
	// outputImage is a gray image here
	cv::cvtColor(inputImage, outputImage, cv::COLOR_RGB2GRAY);
	bool found = false;
	switch(m_state)
	{
		case kInit:
			if (m_clicked)
			{
				m_clicked = false;
				if (m_cmt.initialise(outputImage, m_topLeft, m_bottomDown) == 0)
					m_state = kProcessFrame;
			}
			else
				m_state = kInit; // if there is no feature found in the region go back to selection
			break;
		case kProcessFrame:
			m_cmt.processFrame(outputImage);
			
			for(int i = 0; i < m_cmt.trackedKeypoints.size(); i++)
				cv::circle(inputImage, m_cmt.trackedKeypoints[i].first.pt, 3, cv::Scalar(255,255,255));
			
			cv::line(inputImage, m_cmt.topLeft, m_cmt.topRight, cv::Scalar(0,0,255),2);
			cv::line(inputImage, m_cmt.topRight, m_cmt.bottomRight, cv::Scalar(0,0,255),2);
			cv::line(inputImage, m_cmt.bottomRight, m_cmt.bottomLeft, cv::Scalar(0,0,255),2);
			cv::line(inputImage, m_cmt.bottomLeft, m_cmt.topLeft, cv::Scalar(0,0,255),2);
			if(m_clicked)
			{
				m_state = kInit;
				m_clicked = false;
			}
			else
				m_state = kProcessFrame;
			found = true;
			break;
		default:
			break;
	}
	
	// Todo update position
	return found;
}

void DynamicTracking::event(Event* event)
{
	if (event->type() == Event::MousePressedOnView)
	{
		MouseEvent* e = static_cast<MouseEvent*>(event);
		m_topLeft.x = e->getX();
		m_topLeft.y = e->getY();
	}
	else if (event->type() == Event::MouseReleasedOnview)
	{
		MouseEvent* e = static_cast<MouseEvent*>(event);
		cv::Point2i lastClicked = m_topLeft;
		m_topLeft.x = std::min(e->getX(), lastClicked.x);
		m_topLeft.y = std::min(e->getY(), lastClicked.y);
		m_bottomDown.x = std::max(e->getX(), lastClicked.x);
		m_bottomDown.y = std::max(e->getY(), lastClicked.y);
	
		printf("topLeft is (%f, %f)\n", m_topLeft.x, m_topLeft.y);
		printf("BottomDown is (%f, %f)\n", m_bottomDown.x, m_bottomDown.y);
		m_clicked = true;
	}
}
