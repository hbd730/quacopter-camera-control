//
//  ControlWidgets.m
//  CameraDetection
//
//  Created by Peter Huang on 27/11/2014.
//  Copyright (c) 2014 Peter Huang. All rights reserved.
//

#import "ControlWidgets.h"

@interface ControlWidgets ()
{
	NSSlider* m_kpSlider;
	NSSlider* m_kiSlider;
	NSSlider* m_kdSlider;
	NSSlider* m_dtSlider;
	NSTextField* m_kpTextfield;
	NSTextField* m_kiTextfield;
	NSTextField* m_kdTextfield;
	NSTextField* m_dtTextfield;
}
@end

@implementation ControlWidgets
@synthesize activeControlID;
@synthesize activeValue;
@synthesize associatedObject;

- (id)initWithFrame:(NSRect)frameRect
{
	self = [super initWithFrame:frameRect];
	if (self)
	{
		NSRect frameSliderKp = NSMakeRect(0, 60, 181, 22);
		m_kpSlider = [self createSliderWithFrame:frameSliderKp andTag:kPIDkp];
		NSRect frameSliderKi = NSMakeRect(0, 30, 181, 22);
		m_kiSlider = [self createSliderWithFrame:frameSliderKi andTag:kPIDki];
		NSRect frameSliderKd = NSMakeRect(0, 0, 181, 22);
		m_kdSlider = [self createSliderWithFrame:frameSliderKd andTag:kPIDkd];
//		NSRect frameSliderDt = NSMakeRect(0, 0, 181, 22);
//		m_dtSlider = [self createSliderWithFrame:frameSliderDt andTag:kPIDdt];

		NSRect frameTextKp = NSMakeRect(190, 60, 53, 22);
		m_kpTextfield = [self createTextFieldWithFrame:frameTextKp andTag:kPIDkp];
		NSRect frameTextKi = NSMakeRect(190, 30, 53, 22);
		m_kiTextfield = [self createTextFieldWithFrame:frameTextKi andTag:kPIDki];
		NSRect frameTextKd = NSMakeRect(190, 0, 53, 22);
		m_kdTextfield = [self createTextFieldWithFrame:frameTextKd andTag:kPIDkd];
//		NSRect frameTextDt = NSMakeRect(190, 0, 53, 22);
//		m_dtTextfield = [self createTextFieldWithFrame:frameTextDt andTag:kPIDdt];
	}
	activeControlID = 0;
	
	return self;
}

- (NSSlider*)createSliderWithFrame:(NSRect)frame andTag:(NSInteger)tag
{
	NSSlider* slider = [[NSSlider alloc] initWithFrame:frame];
	[slider setTag:tag];
	[slider setTarget:self];
	[slider setAction: @selector(sliderValueChanged:)];
	[slider setMaxValue:50];
	[self addSubview:slider];
	return slider;
}

- (NSTextField*)createTextFieldWithFrame:(NSRect)frame andTag:(NSInteger)tag
{
	NSTextField* textField = [[NSTextField alloc] initWithFrame:frame];
	[textField setTag:tag];
	[textField setTarget:self];
	[textField setAction: @selector(textFieldValueChanged:)];
	[self addSubview:textField];
	return textField;
}

- (void)sliderValueChanged:(id)sender
{
	float parameterValue = [sender floatValue];
	activeValue = parameterValue;
	switch([sender tag])
	{
		case kPIDkp:
			[m_kpTextfield setFloatValue:parameterValue];
			break;
		case kPIDki:
			[m_kiTextfield setFloatValue:parameterValue];
			break;
		case kPIDkd:
			[m_kdTextfield setFloatValue:parameterValue];
			break;
//		case kPIDdt:
//			[m_dtTextfield setFloatValue:parameterValue];
//			break;
		default:
			break;
	}
	activeControlID = [sender tag];
	[self sendAction:[self action] to:[self target]];
	
}

- (void)textFieldValueChanged:(id)sender
{
	float parameterValue = [sender floatValue];
	[self setFloatValue:parameterValue];
	activeValue = parameterValue;
	switch([sender tag])
	{
		case kPIDkp:
			[m_kpSlider setFloatValue:parameterValue];
			break;
		case kPIDki:
			[m_kiSlider setFloatValue:parameterValue];
			break;
		case kPIDkd:
			[m_kdSlider setFloatValue:parameterValue];
			break;
//		case kPIDdt:
//			[m_dtSlider setFloatValue:parameterValue];
//			break;
		default:
			break;
	}
	activeControlID = [sender tag];
	[self sendAction:[self action] to:[self target]];
}

@end
