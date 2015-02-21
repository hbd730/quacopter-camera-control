//
//  CFRadioController.cpp
//  CameraDetection
//
//  Created by Peter Huang on 9/02/2015.
//  Copyright (c) 2015 Peter Huang. All rights reserved.
//


#include "CFRadioController.h"
#include "CCrazyflie.h"


void CFRadioController::radioTask(string msg)
{
    cout << "Radio thread started " << msg << endl;
	
	while(m_cfCopter->cycle() && m_stopThread == false)
	{
		
		//std::this_thread::sleep_for( std::chrono::seconds(1) );
	}
	cout << "Radio thread terminated!"  << endl;
}

CFRadioController::CFRadioController() noexcept:
	m_crRadio(NULL),
	m_cfCopter(NULL),
	m_stopThread(true),
	m_thread()
{
	m_crRadio = new CCrazyRadio("radio://0/10/250K");
	if(m_crRadio->startRadio())
	{
		m_cfCopter = new CCrazyflie(m_crRadio);
		m_cfCopter->setSendSetpoints(true);
	}
}

CFRadioController::~CFRadioController() noexcept
{
	m_stopThread = true;
	if(m_thread.joinable())
		m_thread.join();
	if(m_cfCopter)
		delete m_cfCopter; // cf instance needs to be destructed first to stop logging data and prevent buffer overflow
	delete m_crRadio;
}

void CFRadioController::start()
{
	if(m_stopThread)
	{
		m_stopThread = false;
		m_thread = thread(&CFRadioController::radioTask, this, "successfully");
	}
}

void CFRadioController::sendParameter(int thrust, float yaw, float pitch, float roll)
{
	if(m_cfCopter)
	{
		m_cfCopter->setThrust(thrust);
		m_cfCopter->setYaw(yaw);
		m_cfCopter->setPitch(pitch);
		m_cfCopter->setRoll(roll);
	}
	cout << "thrust is " << thrust << "yaw is " << yaw \
	<< "pitch is " << pitch << "roll is " << roll << endl;
}
