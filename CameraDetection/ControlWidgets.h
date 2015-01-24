//
//  ControlWidgets.h
//  CameraDetection
//
//  Created by Peter Huang on 27/11/2014.
//  Copyright (c) 2014 Peter Huang. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface ControlWidgets :  NSControl
{
	enum ParameterType: NSInteger
	{
		kPIDkp, kPIDki, kPIDkd, kPIDdt
	};
}
@property (assign,readonly) enum ParameterType activeControlID;
@property (assign,readonly) float activeValue;
@property (assign) id associatedObject;

@end
