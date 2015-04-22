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
@public  
	NSSlider* m_Slider1;
	NSSlider* m_Slider2;
	NSSlider* m_Slider3;
	NSTextField* m_textfield1;
	NSTextField* m_textfield2;
	NSTextField* m_textfield3;
}
@property (assign,readonly) enum ParameterType activeControlID;
@property (assign,readonly) float activeValue;
@property (assign) id associatedObject;
@end


@interface PIDControlWidgets : ControlWidgets
{
}
@end

@interface HSVLowControlWidgets : ControlWidgets
{
}
@end

@interface HSVHighControlWidgets : ControlWidgets
{
}
@end



