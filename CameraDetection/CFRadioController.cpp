//
//  CFRadioController.cpp
//  CameraDetection
//
//  Created by Peter Huang on 9/02/2015.
//  Copyright (c) 2015 Peter Huang. All rights reserved.
//


#include "CFRadioController.h"
#include "CCrazyflie.h"
#include <thread>

void CFRadioController::radioTask(string msg)
{
    cout << "Radio thread started " << msg << endl;
	
	while(1) //m_cfCopter->cycle()
	{
		cout << "we are in the loop" << endl;
		std::this_thread::sleep_for( std::chrono::seconds(1) );
	}
}

CFRadioController::CFRadioController() noexcept:
	m_crRadio(NULL),
	m_cfCopter(NULL)
{
	m_crRadio = new CCrazyRadio("radio://0/10/250K");
	if(m_crRadio->startRadio())
	{
		m_cfCopter = new CCrazyflie(m_crRadio);
		m_cfCopter->setThrust(10001); // test value
	}
	
    thread t1(&CFRadioController::radioTask, this, "successfully");
    t1.join();
}

CFRadioController::~CFRadioController() noexcept
{
	delete m_crRadio;
	if(m_cfCopter)
		delete m_cfCopter;
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
}
