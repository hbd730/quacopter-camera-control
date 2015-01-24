//
//  PIDCalc.h
//  CameraDetection
//
//  Created by Shen He on 15/11/2014.
//  Copyright (c) 2014 FlightDynamics. All rights reserved.
//

#ifndef __CameraDetection__PIDCalc__
#define __CameraDetection__PIDCalc__

class pid_calc_if
{
public:
	pid_calc_if() noexcept = default;
	pid_calc_if(const pid_calc_if&) = delete;
	pid_calc_if(pid_calc_if&&) = delete;
	pid_calc_if& operator = (const pid_calc_if&) = delete;
	pid_calc_if& operator = (pid_calc_if&&) = delete;
	virtual ~pid_calc_if() noexcept = default;
	
	virtual void reset() noexcept = 0;
	virtual float run(float error) noexcept = 0;
	virtual void setKp(float kp) noexcept = 0;
	virtual void setKi(float ki) noexcept = 0;
	virtual void setKd(float kd) noexcept = 0;
	virtual void setDt(float dt) noexcept = 0;
};

class pid_calc_t: public pid_calc_if
{
public:
	pid_calc_t(float kp,
			   float ki,
			   float kd,
			   float dt,
			   float ihigh = 5000.0,
			   float ilow = -5000.0,
			   float pidHighLimit = 5000.0,
			   float pidLowLimit = -5000.0) noexcept;
	~pid_calc_t() noexcept = default;
	
	void reset() noexcept override;
	float run(float error) noexcept override;
	void setKp(float kp) noexcept override;
	void setKi(float ki) noexcept override;
	void setKd(float kd) noexcept override;
	void setDt(float dt) noexcept override;
	
private:
	float kp_;
	float ki_;
	float kd_;
	float dt_;
	const float ilow_;
	const float ihigh_;
	const float pidLowLimit_;
	const float pidHighLimit_;
	float sum_ = 0.0f;
	float error_ = 0.0f;
	unsigned int count_ = 0;
};

#endif /* defined(__CameraDetection__PIDCalc__) */
