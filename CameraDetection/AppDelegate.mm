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

#define CFReleaseSafe(CF) { CFTypeRef _cf = (CF); if (_cf){ (CF) = NULL; CFRelease(_cf); } }
//#define KEYBOARD_CONTROL

@interface AppDelegate ()<CameraDelegate>
{
	TrackingDelegate* m_trackingDelegate;
	PIDCalcThrust* m_thrustPIDCalc;
	PIDCalcRP* m_pitchPIDCalc;
	PIDCalcRP* m_yawPIDCalc;
	PIDCalcRP* m_rollPIDCalc;
	CFRadioController* m_trafficController;
	ControlWidgets* m_controlWidget;
	int m_kPitch;
	int m_kYaw;
	bool stopFlag;
}

@property (assign) IBOutlet NSButton *m_stopButton;
@property (assign) IBOutlet NSView *m_view;
@property (nonatomic, strong) Camera* m_camera;
@property (assign) IBOutlet OpenGLPreview* m_capturePreview;
@property (assign) IBOutlet OpenGLPreview* m_outputPreview;
@property (assign) IBOutlet NSPopUpButton* m_strategyPopup;
@end

@implementation AppDelegate
@synthesize m_view;
@synthesize m_camera;
@synthesize m_capturePreview;
@synthesize m_outputPreview;
@synthesize m_strategyPopup;
@synthesize m_stopButton;

- (void) frameCaptured:(CVImageBufferRef) imageBuffer
{
	// Get information about the image
	uint8_t *baseAddress = (uint8_t *)CVPixelBufferGetBaseAddress(imageBuffer);
	size_t width = CVPixelBufferGetWidth(imageBuffer);  // format kCVPixelFormatType_32BGRA to get use CVPixelBufferGetPixelFormatType
	size_t height = CVPixelBufferGetHeight(imageBuffer);
	size_t stride = CVPixelBufferGetBytesPerRow(imageBuffer);
	
	// Trasform to OpenCV matrix
	cv::Mat currentFrame((int)height, (int)width, CV_8UC4, (void*)baseAddress, stride);
	
	// Start tracking
	cv::Point3i currentPosition = m_trackingDelegate->startTracking(currentFrame);
	
	float thrustError = currentPosition.y - (int)height/2;
	float thrust = m_thrustPIDCalc->run(thrustError);
	
	float rollError = currentPosition.x - (int)width/2;
	float roll = m_rollPIDCalc->run(rollError);
	
#ifdef KEYBOARD_CONTROL
	float pitch = m_kPitch;
	float yaw = m_kYaw;
#else
	float pitchError = currentPosition.z - 300;
	float pitch = m_pitchPIDCalc->run(pitchError);
	float yaw = 0;
#endif
	
	if(stopFlag)
		m_trafficController->sendParameter(0, 0, 0, 0);
	else
		m_trafficController->sendParameter(thrust, yaw, pitch, roll);
	
	// Redering capture preview
	[m_capturePreview renderFromBuffer:imageBuffer];
	
	CVPixelBufferRef outputBuffer = NULL;
	cv::Mat outputImage = m_trackingDelegate->getOutputImage();
	uint8_t *outputAddress = outputImage.data;
	OSStatus error = CVPixelBufferCreateWithBytes(kCFAllocatorDefault, width, height, kCVPixelFormatType_OneComponent8, outputAddress, width, \
												  NULL, NULL, NULL, &outputBuffer);
	if(error)
	{
		NSError *errorCode = [NSError errorWithDomain:NSOSStatusErrorDomain code:error userInfo:nil];
		NSLog(@"Error: %@", [errorCode description]);
	}
	else // Redering output preview
		[m_outputPreview renderFromBuffer:outputBuffer];
	
	CFReleaseSafe(outputBuffer);
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
	m_thrustPIDCalc = new PIDCalcThrust(30, 40, 2.5);
	m_pitchPIDCalc = new PIDCalcRP(0.05, 0.00025, 0.1);
	m_rollPIDCalc = new PIDCalcRP(0.05, 0.00025, 1);
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

	stopFlag = true;
	[m_stopButton setTitle:@"Start"];
	m_camera = [[Camera alloc] init];
	m_camera.delegate = self;
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

- (void)hsvLowValueChanged:(ControlWidgets*)sender
{
	ParameterType type = [sender activeControlID];
	float value = [sender activeValue];
	BallTracking* ballTracker  = m_trackingDelegate->getBallTracker();
	switch(type)
	{
		case kFirst:
			ballTracker->setLowH(value);
			break;
		case kSecond:
			ballTracker->setLowS(value);
			break;
		case kThird:
			ballTracker->setLowV(value);
			break;
		default:
			break;
	}
}

- (void)hsvHighValueChanged:(ControlWidgets*)sender
{
	ParameterType type = [sender activeControlID];
	float value = [sender activeValue];
	BallTracking* ballTracker  = m_trackingDelegate->getBallTracker();
	switch(type)
	{
		case kFirst:
			ballTracker->setHighH(value);
			break;
		case kSecond:
			ballTracker->setHighS(value);
			break;
		case kThird:
			ballTracker->setHighV(value);
			break;
		default:
			break;
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
	if(dynamic_cast<PIDCalcRP*>(obj))
	   [m_controlWidget->m_Slider2 setMaxValue: 0.1];
	[m_view addSubview:m_controlWidget];
}

- (void)addHSVLowGroupWithFrame:(NSRect)frame
{
	m_controlWidget = [[[HSVLowControlWidgets alloc] initWithFrame:frame] autorelease];
	[m_controlWidget setTarget:self];
	[m_controlWidget setAction:@selector(hsvLowValueChanged:)];
	[m_controlWidget->m_Slider1 setIntValue:BallTracking::kLowH];
	[m_controlWidget->m_Slider2 setIntValue:BallTracking::kLowS];
	[m_controlWidget->m_Slider3 setIntValue:BallTracking::kLowV];
	[m_controlWidget->m_textfield1 setIntValue:BallTracking::kLowH];
	[m_controlWidget->m_textfield2 setIntValue:BallTracking::kLowS];
	[m_controlWidget->m_textfield3 setIntValue:BallTracking::kLowV];
	[m_view addSubview:m_controlWidget];
}

- (void)addHSVHighGroupWithFrame:(NSRect)frame
{
	m_controlWidget = [[[HSVHighControlWidgets alloc] initWithFrame:frame] autorelease];
	[m_controlWidget setTarget:self];
	[m_controlWidget setAction:@selector(hsvHighValueChanged:)];
	[m_controlWidget->m_Slider1 setIntValue:BallTracking::kHighH];
	[m_controlWidget->m_Slider2 setIntValue:BallTracking::kHighS];
	[m_controlWidget->m_Slider3 setIntValue:BallTracking::kHighV];
	[m_controlWidget->m_textfield1 setIntValue:BallTracking::kHighH];
	[m_controlWidget->m_textfield2 setIntValue:BallTracking::kHighS];
	[m_controlWidget->m_textfield3 setIntValue:BallTracking::kHighV];
	[m_view addSubview:m_controlWidget];
}

- (void)keyDown:(NSEvent *)theEvent {
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

- (void)keyUp:(NSEvent *)theEvent {
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

@end
