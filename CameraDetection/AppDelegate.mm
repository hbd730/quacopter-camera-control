//
//  AppDelegate.m
//  CameraDetection
//
//  Created by Peter Huang on 18/10/2014.
//  Copyright (c) 2014 FlightDynamics. All rights reserved.
//

#import "TrackingDelegate.h"
#import "AppDelegate.h"
#import "Camera.h"
#import "OpenGLPreview.h"
#include "PIDCalc.h"
#include "CFRadioController.h"
#include "ControlWidgets.h"
#include "Constant.h"
#include "Debug.h"

// To-do List
// 1. try Real-time Compressive Tracking and TLD

@interface AppDelegate ()<CameraDelegate, ViewListener>
{
	TrackingDelegate* m_trackingDelegate;
	PIDCalcThrust* m_thrustPIDCalc;
	PIDCalcRP* m_pitchPIDCalc;
	PIDCalcRP* m_yawPIDCalc;
	PIDCalcRP* m_rollPIDCalc;
	CFRadioController* m_trafficController;
	ControlWidgets* m_controlPIDThrust;
	ControlWidgets* m_controlPIDPitch;
	ControlWidgets* m_controlPIDRoll;
	ControlWidgets* m_controlPIDYaw;
	ControlWidgets* m_controlHSVHigh;
	ControlWidgets* m_controlHSVLow;
	cv::Point3i m_setPoint;
	int m_keyboardPitch;
	int m_keyboardYaw;
	bool m_stopFlag;
	bool m_changeSetPoint;
}

@property (assign) IBOutlet NSButton* m_stopButton;
@property (assign) IBOutlet NSTextField* m_depth;
@property (assign) IBOutlet NSButton* m_setPointCheckBox;
@property (assign) IBOutlet NSView* m_view;
@property (nonatomic, strong) Camera* m_camera;
@property (assign) IBOutlet TrackingPreview* m_capturePreview;
@property (assign) IBOutlet ImageProcessingPreview* m_outputPreview;
@property (assign) IBOutlet NSPopUpButton* m_strategyPopup;
@end

@implementation AppDelegate
@synthesize m_view;
@synthesize m_camera;
@synthesize m_capturePreview;
@synthesize m_outputPreview;
@synthesize m_strategyPopup;
@synthesize m_stopButton;
@synthesize m_depth;
@synthesize m_setPointCheckBox;

- (void)frameCaptured:(CVImageBufferRef)imageBuffer
{
	// Trasform camera buffer to OpenCV matrix
	cv::Mat currentFrame = [self convertCameraBufferToMat:imageBuffer];
	
	// Start tracking
	cv::Point3i currentPosition;
	if (m_trackingDelegate->startTracking(currentFrame, currentPosition) && (m_stopFlag != true))
		[self updatePIDAndSend:currentPosition];
	else
		m_trafficController->sendParameter(0, 0, 0, 0);
	
	// Redering capture preview
	[m_capturePreview renderFromBuffer:imageBuffer];
	
	// Convert OpenCV matrix to preview buffer
	CVPixelBufferRef outputBuffer = [self convertMatToPreviewBuffer:m_trackingDelegate->getOutputImage()];
	
	// Redering output preview
	[m_outputPreview renderFromBuffer:outputBuffer];
	
	CFReleaseSafe(outputBuffer);
}

- (cv::Mat)convertCameraBufferToMat:(CVImageBufferRef)cameraBuffer
{
	// format type kCVPixelFormatType_32BGRA, to query use CVPixelBufferGetPixelFormatType
	uint8_t *baseAddress = (uint8_t *)CVPixelBufferGetBaseAddress(cameraBuffer);
	size_t width = CVPixelBufferGetWidth(cameraBuffer);
	size_t height = CVPixelBufferGetHeight(cameraBuffer);
	size_t stride = CVPixelBufferGetBytesPerRow(cameraBuffer);
	return cv::Mat((int)height, (int)width, CV_8UC4, (void*)baseAddress, stride);
}

- (CVPixelBufferRef)convertMatToPreviewBuffer:(cv::Mat)outputImage
{
	CVPixelBufferRef outputBuffer = NULL;
	uint8_t *outputAddress = outputImage.data;
	size_t width = outputImage.cols;
	size_t height = outputImage.rows;
	OSStatus error = CVPixelBufferCreateWithBytes(kCFAllocatorDefault, width, height, kCVPixelFormatType_OneComponent8, outputAddress, width, \
												  NULL, NULL, NULL, &outputBuffer);
	if(error)
	{
		NSError *errorCode = [NSError errorWithDomain:NSOSStatusErrorDomain code:error userInfo:nil];
		NSLog(@"Error: %@", [errorCode description]);
	}
	return outputBuffer;
}

- (void)updatePIDAndSend:(cv::Point3i)currentPosition
{
	float thrustError = currentPosition.y - m_setPoint.y;
	float thrust = m_thrustPIDCalc->run(thrustError);
	
	float rollError = currentPosition.x - m_setPoint.x;
	float roll = m_rollPIDCalc->run(rollError);
	
	float pitchError = currentPosition.z - m_setPoint.z;
	float pitch = m_pitchPIDCalc->run(pitchError);
	
	float yaw = 0;

	// keyboard can override yaw/pitch value
	yaw = (m_keyboardYaw != 0) ? m_keyboardYaw : yaw;
	pitch = (m_keyboardPitch != 0) ? m_keyboardPitch : pitch;
	m_trafficController->sendParameter(thrust, yaw, pitch, roll);
}

- (void)setPointChanged:(NSPoint)point
{
	m_setPoint.x = point.x;
	m_setPoint.y = point.y;
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
	// Insert code here to initialize your application
	[m_strategyPopup addItemWithTitle:@"Ball Tracking"];
	[m_strategyPopup addItemWithTitle:@"Static Tracking"];
	[m_strategyPopup addItemWithTitle:@"Dynamic Tracking"];
	
	m_trackingDelegate = new TrackingDelegate();
	m_trackingDelegate->setStrategy(TrackingDelegate::kBall);
	m_trafficController = new CFRadioController();
	m_thrustPIDCalc = new PIDCalcThrust(kThrustKp, kThrustKi, kThrustKd);
	m_pitchPIDCalc = new PIDCalcRP(kPitchKp, kPitchKi, kPitchKd);
	m_rollPIDCalc = new PIDCalcRP(kRollKp, kRollKi, kRollKd);
	m_yawPIDCalc = new PIDCalcRP(kYawKp, kYawKi, kYawKd);
	
	m_controlPIDThrust = [self createControlsWithFrame: NSMakeRect(80, 195, 270, 180)];
	[m_controlPIDThrust initControlsWithValues:kThrustKp control2:kThrustKi control3:kThrustKd];
	[m_controlPIDThrust setControlsMaxValues:50 control2:10 control3:20];
	
	m_controlPIDPitch = [self createControlsWithFrame: NSMakeRect(510, 195, 270, 180)];
	[m_controlPIDPitch initControlsWithValues:kPitchKp control2:kPitchKi control3:kPitchKd];
	[m_controlPIDPitch setControlsMaxValues:0.1 control2:0.1 control3:0.01];
	
	m_controlPIDRoll = [self createControlsWithFrame: NSMakeRect(510, 100, 270, 180)];
	[m_controlPIDRoll initControlsWithValues:kRollKp control2:kRollKi control3:kRollKd];
	[m_controlPIDRoll setControlsMaxValues:0.1 control2:0.1 control3:0.01];
	
	m_controlPIDYaw = [self createControlsWithFrame: NSMakeRect(80, 100, 270, 180)];
	[m_controlPIDYaw initControlsWithValues:kYawKp control2:kYawKi control3:kYawKd];
	
	m_controlHSVLow = [self createControlsWithFrame: NSMakeRect(80, 9, 270, 180)];
	[m_controlHSVLow initControlsWithValues:kLowH control2:kLowS control3:kLowV];
	[m_controlHSVLow setControlsMaxValues:180 control2:255 control3:255];
	
	m_controlHSVHigh = [self createControlsWithFrame: NSMakeRect(510, 9, 270, 180)];
	[m_controlHSVHigh initControlsWithValues:kHighH control2:kHighS control3:kHighV];
	[m_controlHSVHigh setControlsMaxValues:180 control2:255 control3:255];
	
	m_setPoint = cv::Point3i(kWidth/2, kHeight/2, kDepth);
	m_stopFlag = true;
	m_changeSetPoint = false;
	[m_depth setIntValue:m_setPoint.z];
	[m_setPointCheckBox setState:NSOffState];
	[m_stopButton setTitle:@"Start"];
	m_camera = [[Camera alloc] init];
	m_camera.delegate = self;
	m_capturePreview.m_viewListener = self;
	[m_view.window makeFirstResponder:self];
	[m_camera startRunning];
}

- (void)applicationWillTerminate:(NSNotification *)aNotification
{
	// Insert code here to tear down your application
	delete m_trackingDelegate;
	delete m_thrustPIDCalc;
	delete m_pitchPIDCalc;
	delete m_yawPIDCalc;
	delete m_rollPIDCalc;
	delete m_trafficController;
	[m_camera stopRunning];
	[m_camera dealloc];
}

- (ControlWidgets*)createControlsWithFrame:(NSRect)frame
{
	ControlWidgets* controlWidgets = [[[ControlWidgets alloc] initWithFrame:frame] autorelease];
	[controlWidgets setTarget:self];
	[controlWidgets setAction:@selector(onAnyChange:)];
	[m_view addSubview:controlWidgets];
	return controlWidgets;
}

- (void)onAnyChange:(ControlWidgets*)sender
{
	ParameterType parameterID = [sender activeControlID];
	float value = [sender activeValue];
	if(sender == m_controlPIDThrust)
	{
		m_thrustPIDCalc->setGain((IPIDCalc::PIDGainType)parameterID, value);
	}
	else if(sender == m_controlPIDRoll)
	{
		m_rollPIDCalc->setGain((IPIDCalc::PIDGainType)parameterID, value);
	}
	else if(sender == m_controlPIDPitch)
	{
		m_pitchPIDCalc->setGain((IPIDCalc::PIDGainType)parameterID, value);
	}
	else if(sender == m_controlHSVHigh)
	{
		BallTrackerEvent event(UIControlEvent::HSVLowGroupChanged, parameterID, value);
		m_trackingDelegate->onParameterChanged(event);
	}
	else if(sender == m_controlHSVLow)
	{
		BallTrackerEvent event(UIControlEvent::HSVHighGroupChanged, parameterID, value);
		m_trackingDelegate->onParameterChanged(event);
	}
}

- (IBAction)strategyChanged:(id)sender
{
	TrackingDelegate::StrategyType type;
	type = (TrackingDelegate::StrategyType)[m_strategyPopup indexOfSelectedItem];
	m_trackingDelegate->setStrategy(type);
}

- (IBAction)stopAndReset:(id)sender
{
	m_stopFlag = !m_stopFlag;
	if(m_stopFlag)
		[m_stopButton setTitle:@"Start"];
	else
	{
		m_trafficController->start();
		[m_stopButton setTitle:@"Stop"];
	}
}

- (BOOL)acceptsFirstResponder
{
    return YES;
}

- (void)keyDown:(NSEvent *)theEvent
{
	// Arrow keys are associated with the numeric keypad
	if ([theEvent modifierFlags] & NSNumericPadKeyMask)
	{
		NSString *theArrow = [theEvent charactersIgnoringModifiers];
		unichar keyChar = 0;
		if ([theArrow length] == 1)
		{
			keyChar = [theArrow characterAtIndex:0];
			switch (keyChar)
			{
				case NSUpArrowFunctionKey:
					m_keyboardPitch = 8;
					break;
				case NSLeftArrowFunctionKey:
					m_keyboardYaw = 10;
					break;
				case NSRightArrowFunctionKey:
					m_keyboardYaw = -10;
					break;
				case NSDownArrowFunctionKey:
					m_keyboardPitch = -8;
					break;
			}
		}
	}
	else
		[super keyDown:theEvent];
}

- (void)keyUp:(NSEvent *)theEvent
{
	// Arrow keys are associated with the numeric keypad
	if ([theEvent modifierFlags] & NSNumericPadKeyMask)
	{
		NSString *theArrow = [theEvent charactersIgnoringModifiers];
		unichar keyChar = 0;
		if ([theArrow length] == 1)
		{
			keyChar = [theArrow characterAtIndex:0];
			m_keyboardPitch = 0;
			m_keyboardYaw = 0;
		}
	}
	else
	{
		[super keyDown:theEvent];
	}
}

- (IBAction)depthChanged:(NSTextField*)sender
{
	m_setPoint.z = [sender intValue];
}

- (IBAction)setPointSelectionChanged:(NSButton*)sender
{
	if(sender.state == NSOnState)
		m_changeSetPoint = true;
	else
		m_changeSetPoint = false;
}

- (void)viewEventHandler:(NSEvent*)theEvent
{
	NSPoint clickedPoint;
	switch(theEvent.type)
	{
		case NSLeftMouseDown:
			clickedPoint = [m_capturePreview transform:[theEvent locationInWindow]];
			m_trackingDelegate->getCurrentTracker()->setSelectedRegion(clickedPoint.x, clickedPoint.y, true);
			if(m_changeSetPoint)
				[self setPointChanged:clickedPoint];
			break;
		case NSLeftMouseUp:
			clickedPoint = [m_capturePreview transform:[theEvent locationInWindow]];
			m_trackingDelegate->getCurrentTracker()->setSelectedRegion(clickedPoint.x, clickedPoint.y, false);
			break;
		default:
			break;
	}
}

@end
