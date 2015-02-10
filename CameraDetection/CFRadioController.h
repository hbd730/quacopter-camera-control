//
//  CFRadioController.h
//  CameraDetection
//
//  Created by Peter Huang on 9/02/2015.
//  Copyright (c) 2015 Peter Huang. All rights reserved.
//

#pragma once

#include "DataTraffic.h"

class CFRadioController: public IDataTraffic
{
	
public:
	CFRadioController() noexcept;
	CFRadioController(const CFRadioController&) = delete;
	CFRadioController(CFRadioController&&) = delete;
	CFRadioController& operator = (const CFRadioController&) = delete;
	CFRadioController& operator = (CFRadioController&&) = delete;
	virtual ~CFRadioController() noexcept = default;
	virtual void sendParameter(int thrust, float yaw, float pitch, float roll);
};
