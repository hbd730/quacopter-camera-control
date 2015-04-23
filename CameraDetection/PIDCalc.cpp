//
//  PIDCalc.cpp
//  CameraDetection
//
//  Created by Peter Huang on 15/11/2014.
//  Copyright (c) 2014 FlightDynamics. All rights reserved.
//

#include "PIDCalc.h"
#include "utils.h"

void IPIDCalc::setGain(PIDGainType type, float value)
{
	switch(type)
	{
		case kp:
			m_kp = value;
			break;
		case ki:
			m_ki = value;
			break;
		case kd:
			m_kd = value;
			break;
		default:
			break;
	}
};

float PIDCalcThrust::run(float error) noexcept
{
	m_sum += error * kDt;
	m_sum = bound(m_sum, kILow, kIHigh);
	float deriv = 0.0f;
	if(kDt)
	{
		if((error - m_previousError) > 0) // means a significant drop
			deriv = (error - m_previousError) / kDt;
		else
			deriv = 0;
	}

	m_previousError = error;
	float pidValue = m_kp * m_previousError + m_ki * m_sum + m_kd * deriv;
	
	int thrustValue = pidValue + kThrustOffset;
	thrustValue = bound(thrustValue, kMinThrust, kMaxThrust);
	
	return thrustValue;
}

float PIDCalcRP::run(float error) noexcept
{
	m_sum += error * kDt;
	m_sum = bound(m_sum, kILow, kIHigh);
	float deriv = 0.0f;
	
	if(kDt)
		deriv = (error - m_previousError) / kDt;
		
	m_previousError = error;
	
	float pidValue = m_kp * m_previousError + m_ki * m_sum + m_kd * deriv;
	pidValue = bound(pidValue, kMinRoll, kMaxRoll);
		
	return pidValue;
}
