//
//  ZeroMqController.h
//  CameraDetection
//
//  Created by Peter Huang on 15/11/2014.
//  Copyright (c) 2014 FlightDynamics. All rights reserved.
//

#pragma once

#include "DataTraffic.h"

#ifdef ZEROMQ_SUPPORTED
#include <zmq.hpp>

//
// This class use zeroMq to achieve interprocess communication with
// crazyflie-client python script detection.py which use libusb to drive crazyradio
//
class ZeroMqController: public IDataTraffic
{
public:
	ZeroMqController() noexcept;
	ZeroMqController(const ZeroMqController&) = delete;
	ZeroMqController(ZeroMqController&&) = delete;
	ZeroMqController& operator = (const ZeroMqController&) = delete;
	ZeroMqController& operator = (ZeroMqController&&) = delete;
	virtual ~ZeroMqController() noexcept = default;
	virtual void sendParameter(int thrust, float yaw, float pitch, float roll);
	
private:
	zmq::context_t context;
	zmq::socket_t publisher;
};

#endif