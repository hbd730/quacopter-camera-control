//
//  ControlWidgets.h
//  CameraDetection
//
//  Created by Peter Huang on 27/11/2014.
//  Copyright (c) 2014 Peter Huang. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#include "Constant.h"

@interface ControlWidgets : NSControl
{ 
	NSSlider* m_Slider1;
	NSSlider* m_Slider2;
	NSSlider* m_Slider3;
	NSTextField* m_textfield1;
	NSTextField* m_textfield2;
	NSTextField* m_textfield3;
}

@property (assign,readonly) enum ParameterType activeControlID;
@property (assign,readonly) float activeValue;

- (void)initControlsWithValues:(float)value1 control2:(float)value2 control3:(float)value3;
- (void)setControlsMaxValues:(float)value1 control2:(float)value2 control3:(float)value3;

@end

