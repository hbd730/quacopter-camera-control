//
//  PIDCalc.cpp
//  CameraDetection
//
//  Created by Shen He on 15/11/2014.
//  Copyright (c) 2014 FlightDynamics. All rights reserved.
//

#include "PIDCalc.h"
#include "utils.h"

pid_calc_t::pid_calc_t(float kp, float ki, float kd, float dt, float ihigh, float ilow, float pidHighLimit, float pidLowLimit) noexcept:
kp_(kp),
ki_(ki),
kd_(kd),
dt_(dt),
ilow_(ilow),
ihigh_(ihigh),
pidLowLimit_(pidLowLimit),
pidHighLimit_(pidHighLimit)
{
	;
}

void pid_calc_t::reset() noexcept
{
	sum_ = 0.0f;
	error_ = 0.0f;
	count_ = 0;
	kp_ = 0;
	ki_ = 0;
	kd_ = 0;
	dt_ = 0;
}

float pid_calc_t::run(float error) noexcept
{
	sum_ += error*dt_;
	sum_ = bound(sum_, ilow_, ihigh_);
	
	float deriv = 0.0f;
	if (count_ != 0)
	{
		if(dt_ != 0)
			deriv = (error - error_)/dt_;
	}
	++count_;
	error_ = error;
	
	float pidValue = kp_*error_ + ki_*sum_ + kd_*deriv;
	pidValue = bound(pidValue, pidLowLimit_, pidHighLimit_);
	
	return pidValue;
}

void pid_calc_t::setKp(float kp) noexcept
{
	kp_ = kp;
	printf("kp is %f\n",kp_);
}

void pid_calc_t::setKi(float ki) noexcept
{
	ki_ = ki;
	printf("ki is %f\n",ki_);
}

void pid_calc_t::setKd(float kd) noexcept
{
	kd_ = kd;
	printf("kd is %f\n",kd_);
}

void pid_calc_t::setDt(float dt) noexcept
{
	dt_ = dt;
	printf("dt is %f\n",dt_);
}
