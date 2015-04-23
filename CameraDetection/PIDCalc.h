//
//  PIDCalc.h
//  CameraDetection
//
//  Created by Peter Huang on 15/11/2014.
//  Copyright (c) 2014 FlightDynamics. All rights reserved.
//

#pragma once

// There are three form of PID controller: standard/classic/independent
// Here we use independent form where 1/dt is sampling frequency, which is 29.97fps
class IPIDCalc
{
public:
	enum PIDGainType
	{
		kp,
		ki,
		kd
	};
	IPIDCalc(float kp, float ki, float kd) noexcept
	:m_kp(kp), m_ki(ki), m_kd(kd), m_sum(0), m_previousError(0) {};
		
	IPIDCalc(const IPIDCalc&) = delete;
	IPIDCalc(IPIDCalc&&) = delete;
	IPIDCalc& operator = (const IPIDCalc&) = delete;
	IPIDCalc& operator = (IPIDCalc&&) = delete;
	virtual ~IPIDCalc() noexcept = default;
	virtual float run(float error) noexcept = 0; // run outputs the final raw data sent to the radio Link
	void reset() noexcept {m_sum = 0.0f; m_previousError = 0.0f; m_kp = 0; m_ki = 0; m_kd = 0;};
	void setGain(PIDGainType type, float value);
	
protected:
	float m_kp;
	float m_ki;
	float m_kd;
	float m_sum;
	float m_previousError;
};

// This is Thrust PID calulator
class PIDCalcThrust: public IPIDCalc
{
public:
	PIDCalcThrust(float kp, float ki, float kd) noexcept
	:IPIDCalc(kp, ki, kd) {};
	~PIDCalcThrust() noexcept = default;
	float run(float error) noexcept override;
	
private:
	static constexpr float kIHigh = 3000.0;
	static constexpr float kILow  = -1200.0;
	static constexpr float kDt    = 0.03;
	static constexpr int kThrustOffset = 38000;
	static constexpr int kMinThrust = 0;
	static constexpr int kMaxThrust = 65000;
};

// This is Roll/Pitch PID calulator
class PIDCalcRP: public IPIDCalc
{
public:
	PIDCalcRP(float kp, float ki, float kd) noexcept
	:IPIDCalc(kp, ki, kd) {};
	~PIDCalcRP() noexcept = default;
	float run(float error) noexcept override;
	
private:
	static constexpr float kIHigh = 606.0;
	static constexpr float kILow  = -606.0;
	static constexpr float kDt    = 0.03;
	static constexpr float kMinRoll = -15.0;
	static constexpr float kMaxRoll = 15.0;
};

