//
//  ZeroMqController.cpp
//  CameraDetection
//
//  Created by Peter Huang on 15/11/2014.
//  Copyright (c) 2014 FlightDynamics. All rights reserved.
//

#include "ZeroMqController.h"
#include <stdio.h>

#ifdef ZEROMQ_SUPPORTED

ZeroMqController::ZeroMqController() noexcept
:context(1),publisher(context,ZMQ_PUB)
{
	// Prepare our context and publisher
	publisher.bind("tcp://*:5555");
}

void ZeroMqController::sendParameter(int thrust, float yaw, float pitch, float roll)
{
	// Send message to all subscribers
	zmq::message_t message(24);
	memset(message.data(),0,24);
	int dummyEnding = 0;
	sprintf ((char *) message.data(), "%5d %4.2f %4.2f %4.2f %d", thrust, yaw, pitch, roll, dummyEnding);
	printf("thrust is: %5d yaw is %4.2f pitch is %4.2f roll is %4.2f\n", thrust, yaw, pitch, roll);
	publisher.send(message);
}
#endif