//
//  PIDCalc.cpp
//  CameraDetection
//
//  Created by Peter Huang on 15/11/2014.
//  Copyright (c) 2014 FlightDynamics. All rights reserved.
//

#include "PIDCalc.h"
#include "utils.h"

float PIDCalcThrust::run(float error) noexcept
{
	sum_ += error * kDt;
	sum_ = bound(sum_, kILow, kIHigh);
	float deriv = 0.0f;
	if(kDt)
	{
		if((error - error_) <= 0)
			deriv = (error - error_) / kDt;
		else
			deriv = 0;
	}

	error_ = error;
	float pidValue = kp_ * error_ + ki_ * sum_ + kd_ * deriv;
	
	int thrustValue = pidValue + kThrustOffset;
	bound(thrustValue, kMinThrust, kMaxThrust);
	
	return thrustValue;
}

float PIDCalcRP::run(float error) noexcept
{
	sum_ += error * kDt;
	sum_ = bound(sum_, kILow, kIHigh);
	float deriv = 0.0f;
	
	if(kDt)
		deriv = (error - error_) / kDt;
		
	error_ = error;
	
	float pidValue = kp_ * error_ + ki_ * sum_ + kd_ * deriv;
	bound(pidValue, kMinRoll, kMaxRoll);
		
	return pidValue;
}