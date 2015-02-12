//
//  DataTraffic.h
//  CameraDetection
//
//  Created by Peter Huang on 15/11/2014.
//  Copyright (c) 2014 FlightDynamics. All rights reserved.
//

#pragma once
#include <iostream>

class IDataTraffic
{

public:
	IDataTraffic() noexcept{};
	IDataTraffic(const IDataTraffic&) = delete;
	IDataTraffic(IDataTraffic&&) = delete;
	IDataTraffic& operator = (const IDataTraffic&) = delete;
	IDataTraffic& operator = (IDataTraffic&&) = delete;
	virtual ~IDataTraffic() noexcept = default;
	virtual void sendParameter(int thrust, float yaw, float pitch, float roll) = 0;

protected:
	void test(){printf("from base class\n");};
};
