//
//  PIDCalc.h
//  CameraDetection
//
//  Created by Peter Huang on 15/11/2014.
//  Copyright (c) 2014 FlightDynamics. All rights reserved.
//

#pragma once

class IPIDCalc
{
public:
	IPIDCalc(float kp, float ki, float kd) noexcept
	:kp_(kp), ki_(ki), kd_(kd), sum_(0), error_(0) {};
		
	IPIDCalc(const IPIDCalc&) = delete;
	IPIDCalc(IPIDCalc&&) = delete;
	IPIDCalc& operator = (const IPIDCalc&) = delete;
	IPIDCalc& operator = (IPIDCalc&&) = delete;
	virtual ~IPIDCalc() noexcept = default;
	
	virtual float run(float error) noexcept = 0; // run outputs the final raw data sent to the radio Link, client does not need to consider boundary of PID value
	void reset() noexcept {sum_ = 0.0f; error_ = 0.0f; kp_ = 0; ki_ = 0; kd_ = 0;};
	void setKp(float value) {kp_ = value;};
	void setKi(float value) {ki_ = value;};
	void setKd(float value) {kd_ = value;};
	float getKp() const {return kp_;};
	float getKi() const {return ki_;};
	float getKd() const {return kd_;};
	
protected:
	float kp_;
	float ki_;
	float kd_;
	float sum_;
	float error_;
};

// this is Thrust PID calulator
class PIDCalcThrust: public IPIDCalc
{
public:
	PIDCalcThrust(float kp, float ki, float kd) noexcept
	:IPIDCalc(kp, ki, kd) {};
	~PIDCalcThrust() noexcept = default;
	float run(float error) noexcept override;
	
private:
	static constexpr float kIHigh = 300.0;
	static constexpr float kILow  = -200.0;
	static constexpr float kDt    = 0.005;
	const int kThrustOffset = 38000;
	const int kMinThrust = 0;
	const int kMaxThrust = 65000;
};

// this is Roll/Pitch PID calulator
class PIDCalcRP: public IPIDCalc
{
public:
	PIDCalcRP(float kp, float ki, float kd) noexcept
	:IPIDCalc(kp, ki, kd) {};
	~PIDCalcRP() noexcept = default;
	float run(float error) noexcept override;
	
private:
	static constexpr float kIHigh = 30.0;
	static constexpr float kILow  = -30.0;
	static constexpr float kDt    = 0.005; 
	const float kMinRoll = -15.0;
	const float kMaxRoll = 15.0;

};

