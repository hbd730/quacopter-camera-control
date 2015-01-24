//
//  TracfficController.cpp
//  CameraDetection
//
//  Created by Peter Huang on 15/11/2014.
//  Copyright (c) 2014 FlightDynamics. All rights reserved.
//

#include "TracfficController.h"
#include <stdio.h>

TracfficController::TracfficController() noexcept
:context(1),publisher(context,ZMQ_PUB)
{
	// Prepare our context and publisher
	publisher.bind("tcp://*:5555");
}

void TracfficController::sendParameter(int thrust, int yaw, int pitch, int roll)
{
	// Send message to all subscribers
	zmq::message_t message(20);
	memset(message.data(),0,20);
	int dummyEnding = 0;
	snprintf ((char *) message.data(), 20, "%d %d %d %d %d", thrust, yaw, pitch, roll, dummyEnding);
	printf("thrust is: %d yaw is %d pitch is %d roll is %d\n", thrust, yaw, pitch, roll);
	publisher.send(message);
}