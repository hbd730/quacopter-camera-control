//
//  ControlWidgets.m
//  CameraDetection
//
//  Created by Peter Huang on 27/11/2014.
//  Copyright (c) 2014 Peter Huang. All rights reserved.
//

#import "ControlWidgets.h"

@implementation ControlWidgets

@synthesize activeControlID;
@synthesize activeValue;

- (id)initWithFrame:(NSRect)frameRect
{
	self = [super initWithFrame:frameRect];
	if(self)
	{
		NSRect frameSlider1 = NSMakeRect(0, 60, 181, 22);
		m_Slider1 = [self createSliderWithFrame:frameSlider1 andTag:kFirst];
		NSRect frameSlider2 = NSMakeRect(0, 30, 181, 22);
		m_Slider2 = [self createSliderWithFrame:frameSlider2 andTag:kSecond];
		NSRect frameSlider3 = NSMakeRect(0, 0, 181, 22);
		m_Slider3 = [self createSliderWithFrame:frameSlider3 andTag:kThird];
		
		NSRect frameText1 = NSMakeRect(190, 60, 53, 22);
		m_textfield1 = [self createTextFieldWithFrame:frameText1 andTag:kFirst];
		NSRect frameText2 = NSMakeRect(190, 30, 53, 22);
		m_textfield2 = [self createTextFieldWithFrame:frameText2 andTag:kSecond];
		NSRect frameText3 = NSMakeRect(190, 0, 53, 22);
		m_textfield3 = [self createTextFieldWithFrame:frameText3 andTag:kThird];
	}
	activeControlID = 0;
	return self;
}

- (void)initControlsWithValues:(float)value1 control2:(float)value2 control3:(float)value3
{
	[m_Slider1 setFloatValue:value1];
	[m_Slider2 setFloatValue:value2];
	[m_Slider3 setFloatValue:value3];
	[m_textfield1 setFloatValue:value1];
	[m_textfield2 setFloatValue:value2];
	[m_textfield3 setFloatValue:value3];
}

- (void)setControlsMaxValues:(float)value1 control2:(float)value2 control3:(float)value3
{
	[m_Slider1 setMaxValue: value1];
	[m_Slider2 setMaxValue: value2];
	[m_Slider3 setMaxValue: value3];
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
		case kFirst:
			[m_textfield1 setFloatValue:parameterValue];
			break;
		case kSecond:
			[m_textfield2 setFloatValue:parameterValue];
			break;
		case kThird:
			[m_textfield3 setFloatValue:parameterValue];
			break;
		default:
			break;
	}
	activeControlID = (int)[sender tag];
	[self sendAction:[self action] to:[self target]];
	
}

- (void)textFieldValueChanged:(id)sender
{
	float parameterValue = [sender floatValue];
	[self setFloatValue:parameterValue];
	activeValue = parameterValue;
	switch([sender tag])
	{
		case kFirst:
			[m_Slider1 setFloatValue:parameterValue];
			break;
		case kSecond:
			[m_Slider2 setFloatValue:parameterValue];
			break;
		case kThird:
			[m_Slider3 setFloatValue:parameterValue];
			break;
		default:
			break;
	}
	activeControlID = (int)[sender tag];
	[self sendAction:[self action] to:[self target]];
}

@end
