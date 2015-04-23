//
//  Constant.h
//  CameraDetection
//
//  Created by Peter Huang on 21/04/2015.
//  Copyright (c) 2015 Peter Huang. All rights reserved.
//

#pragma once

enum HSVMatrixs   // Online caculator use range H = 0- 360, S = 0-100, V = 0-100
{				  // In OpenCV, H = 0-180, S = 0-255, V = 0-255
	kLowH = 22,
	kHighH = 57,
	kLowS = 34,
	kHighS = 225,
	kLowV = 104,
	kHighV = 255
};

enum CameraResolution
{
	kWidth = 1280,
	kHeight = 720
};

const int kDepth = 300;

enum ParameterType
{
	kFirst, kSecond, kThird
};

const float kThrustKp = 27;
const float kThrustKi = 4.3;
const float kThrustKd = 10.5;

const float kPitchKp = 0.05;
const float kPitchKi = 0.008;
const float kPitchKd = 0.005;

const float kRollKp = 0.04;
const float kRollKi = 0.0002;
const float kRollKd = 0.0075;

const float kYawKp = 0;
const float kYawKi = 0;
const float kYawKd = 0;

