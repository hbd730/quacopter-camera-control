//
//  TracfficController.h
//  CameraDetection
//
//  Created by Peter Huang on 15/11/2014.
//  Copyright (c) 2014 FlightDynamics. All rights reserved.
//

#pragma once
#include <zmq.hpp>

class TracfficController
{
public:
	TracfficController() noexcept;
	TracfficController(const TracfficController&) = delete;
	TracfficController(TracfficController&&) = delete;
	TracfficController& operator = (const TracfficController&) = delete;
	TracfficController& operator = (TracfficController&&) = delete;
	virtual ~TracfficController() noexcept = default;
	
	void sendParameter(int thrust, int yaw, int pitch, int roll);
private:
	zmq::context_t context;
	zmq::socket_t publisher;
};
