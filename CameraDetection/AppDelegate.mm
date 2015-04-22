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
	ControlWidgets* m_controlWidget;
	cv::Point3i m_setPoint;
	int m_kPitch;
	int m_kYaw;
	bool stopFlag;
	bool changeSetPoint;
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
	if (m_trackingDelegate->startTracking(currentFrame, currentPosition) && (stopFlag != true))
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
	uint8_t *baseAddress = (uint8_t *)CVPixelBufferGetBaseAddress(cameraBuffer);
	size_t width = CVPixelBufferGetWidth(cameraBuffer);  // format kCVPixelFormatType_32BGRA to get use CVPixelBufferGetPixelFormatType
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
	
	float yaw = 0; // yaw is not controlled

	yaw = (m_kYaw != 0) ? m_kYaw : yaw;  // keyboard can override yaw/pitch value
	pitch = (m_kPitch != 0) ? m_kPitch : pitch;
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
	m_yawPIDCalc = new PIDCalcRP(0, 0, 0);
	
	NSRect thrustFrame = NSMakeRect(80, 195, 270, 180);
	[self addPIDGroupWithFrame:thrustFrame andCalObject:m_thrustPIDCalc];
	NSRect pitchFrame = NSMakeRect(510, 195, 270, 180);
	[self addPIDGroupWithFrame:pitchFrame andCalObject:m_pitchPIDCalc];
	NSRect yawFrame = NSMakeRect(80, 100, 270, 180);
	[self addPIDGroupWithFrame:yawFrame andCalObject:m_yawPIDCalc];
    NSRect	rollFrame = NSMakeRect(510, 100, 270, 180);
	[self addPIDGroupWithFrame:rollFrame andCalObject:m_rollPIDCalc];
	
	NSRect hsvLowFrame = NSMakeRect(80, 9, 270, 180);
	[self addHSVLowGroupWithFrame:hsvLowFrame];
	NSRect hsvHighFrame = NSMakeRect(510, 9, 270, 180);
	[self addHSVHighGroupWithFrame:hsvHighFrame];

	m_setPoint = cv::Point3i(1280/2, 720/2, 300); // initialise setpoint
	stopFlag = true;
	changeSetPoint = false;
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

- (IBAction)strategyChanged:(id)sender
{
	TrackingDelegate::StrategyType type;
	type = (TrackingDelegate::StrategyType)[m_strategyPopup indexOfSelectedItem];
	m_trackingDelegate->setStrategy(type);
}

- (void)pidValueChanged:(ControlWidgets*)sender
{
	ParameterType type = [sender activeControlID];
	IPIDCalc* cal = (IPIDCalc*)[sender associatedObject];
	float value = [sender activeValue];
	switch(type)
	{
		case kFirst:
			cal->setKp(value);
			break;
		case kSecond:
			cal->setKi(value);
			break;
		case kThird:
			cal->setKd(value);
			break;
		default:
			break;
	}
}

- (void)hsvGroupValueChanged:(ControlWidgets*)sender
{
	ParameterType parameterID = [sender activeControlID];
	int value = [sender activeValue];
	
	if([sender isMemberOfClass:[HSVLowControlWidgets class]])
	{
		BallTrackerEvent event(UIControlEvent::HSVLowGroupChanged, parameterID, value);
		m_trackingDelegate->onParameterChanged(event);
	}
	else if([sender isMemberOfClass:[HSVHighControlWidgets class]])
	{
		BallTrackerEvent event(UIControlEvent::HSVHighGroupChanged, parameterID, value);
		m_trackingDelegate->onParameterChanged(event);
	}
}

- (IBAction)stopAndReset:(id)sender
{
	stopFlag = !stopFlag;
	if(stopFlag)
		[m_stopButton setTitle:@"Start"];
	else
	{
		m_trafficController->start();
		[m_stopButton setTitle:@"Stop"];
	}
}

- (void)addPIDGroupWithFrame:(NSRect)frame andCalObject:(IPIDCalc*)obj
{
	m_controlWidget = [[[PIDControlWidgets alloc] initWithFrame:frame] autorelease];
	[m_controlWidget setTarget:self];
	[m_controlWidget setAction:@selector(pidValueChanged:)];
	[m_controlWidget->m_Slider1 setFloatValue:obj->getKp()];
	[m_controlWidget->m_Slider2 setFloatValue:obj->getKi()];
	[m_controlWidget->m_Slider3 setFloatValue:obj->getKd()];
	[m_controlWidget->m_textfield1 setFloatValue:obj->getKp()];
	[m_controlWidget->m_textfield2 setFloatValue:obj->getKi()];
	[m_controlWidget->m_textfield3 setFloatValue:obj->getKd()];
	m_controlWidget.associatedObject = (id)obj;
	if(dynamic_cast<PIDCalcThrust*>(obj))
	{
		[m_controlWidget->m_Slider2 setMaxValue: 10];
		[m_controlWidget->m_Slider3 setMaxValue: 20];
	}
	if(dynamic_cast<PIDCalcRP*>(obj))
	{
		[m_controlWidget->m_Slider1 setMaxValue: 0.1];
		[m_controlWidget->m_Slider2 setMaxValue: 0.1];
		[m_controlWidget->m_Slider3 setMaxValue: 0.01];
	}
	[m_view addSubview:m_controlWidget];
}

- (void)addHSVLowGroupWithFrame:(NSRect)frame
{
	m_controlWidget = [[[HSVLowControlWidgets alloc] initWithFrame:frame] autorelease];
	[m_controlWidget setTarget:self];
	[m_controlWidget setAction:@selector(hsvGroupValueChanged:)];
	[m_controlWidget->m_Slider1 setIntValue:HSVMatrixs::kLowH];
	[m_controlWidget->m_Slider2 setIntValue:HSVMatrixs::kLowS];
	[m_controlWidget->m_Slider3 setIntValue:HSVMatrixs::kLowV];
	[m_controlWidget->m_textfield1 setIntValue:HSVMatrixs::kLowH];
	[m_controlWidget->m_textfield2 setIntValue:HSVMatrixs::kLowS];
	[m_controlWidget->m_textfield3 setIntValue:HSVMatrixs::kLowV];
	[m_view addSubview:m_controlWidget];
}

- (void)addHSVHighGroupWithFrame:(NSRect)frame
{
	m_controlWidget = [[[HSVHighControlWidgets alloc] initWithFrame:frame] autorelease];
	[m_controlWidget setTarget:self];
	[m_controlWidget setAction:@selector(hsvGroupValueChanged:)];
	[m_controlWidget->m_Slider1 setIntValue:HSVMatrixs::kHighH];
	[m_controlWidget->m_Slider2 setIntValue:HSVMatrixs::kHighS];
	[m_controlWidget->m_Slider3 setIntValue:HSVMatrixs::kHighV];
	[m_controlWidget->m_textfield1 setIntValue:HSVMatrixs::kHighH];
	[m_controlWidget->m_textfield2 setIntValue:HSVMatrixs::kHighS];
	[m_controlWidget->m_textfield3 setIntValue:HSVMatrixs::kHighV];
	[m_view addSubview:m_controlWidget];
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
					m_kPitch = 8;
					break;
				case NSLeftArrowFunctionKey:
					m_kYaw = 10;
					break;
				case NSRightArrowFunctionKey:
					m_kYaw = -10;
					break;
				case NSDownArrowFunctionKey:
					m_kPitch = -8;
					break;
			}
		}
	}
	else
	{
		[super keyDown:theEvent];
	}
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
			m_kPitch = 0;
			m_kYaw = 0;
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
		changeSetPoint = true;
	else
		changeSetPoint = false;
}

- (void)viewEventHandler:(NSEvent*)theEvent
{
	NSPoint clickedPoint;
	switch(theEvent.type)
	{
		case NSLeftMouseDown:
			clickedPoint = [m_capturePreview transform:[theEvent locationInWindow]];
			m_trackingDelegate->getCurrentTracker()->setSelectedRegion(clickedPoint.x, clickedPoint.y, true);
			if(changeSetPoint)
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
