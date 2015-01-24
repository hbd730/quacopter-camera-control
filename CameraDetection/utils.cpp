//
//  utils.cpp
//  CameraDetection
//
//  Created by Shen He on 15/11/2014.
//  Copyright (c) 2014 FlightDynamics. All rights reserved.
//

#include "utils.h"

float normalize_angle(float angle) noexcept
{
	if (angle > 180.0f)
	{
		angle -= 360.0f;
	}
	else if (angle < -180.0f)
	{
		angle += 360.0f;
	}
	else
	{
		;
	}
	return angle;
}