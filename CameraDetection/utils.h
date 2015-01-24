//
//  utils.h
//  CameraDetection
//
//  Created by Shen He on 15/11/2014.
//  Copyright (c) 2014 FlightDynamics. All rights reserved.
//

#ifndef __CameraDetection__utils__
#define __CameraDetection__utils__

#include <algorithm>

float normalize_angle(float angle) noexcept;

template <typename T>
T bound(T val, T low, T high) noexcept
{
	val = std::min(val, high);
	val = std::max(val, low);
	return val;
}

#endif /* defined(__CameraDetection__utils__) */
