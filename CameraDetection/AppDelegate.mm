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
#include "TracfficController.h"
#include "ControlWidgets.h"

@interface AppDelegate ()<CameraDelegate>
{
	TrackingDelegate* m_trackingDelegate;
	pid_calc_t* m_thrustPIDCalc;
	pid_calc_t* m_pitchPIDCalc;
	pid_calc_t* m_yawPIDCalc;
	pid_calc_t* m_rollPIDCalc;
	TracfficController* m_trafficController;
	ControlWidgets* m_controlWidget;
	bool stopFlag;
}
@property (assign) IBOutlet NSButton *m_stopButton;
@property (assign) IBOutlet NSView *m_view;
@property (nonatomic, strong) Camera* m_camera;
@property (assign) IBOutlet OpenGLPreview* m_preview;
@property (assign) IBOutlet NSPopUpButton* m_strategyPopup;
@end

@implementation AppDelegate
@synthesize m_view;
@synthesize m_camera;
@synthesize m_preview;
@synthesize m_strategyPopup;
@synthesize m_stopButton;

- (void) frameCaptured:(CVImageBufferRef) imageBuffer
{
	// Get information about the image
	uint8_t *baseAddress = (uint8_t *)CVPixelBufferGetBaseAddress(imageBuffer);
	size_t width = CVPixelBufferGetWidth(imageBuffer);
	size_t height = CVPixelBufferGetHeight(imageBuffer);
	size_t stride = CVPixelBufferGetBytesPerRow(imageBuffer);
	
	// Trasform to OpenCV matrix
	cv::Mat currentFrame((int)height, (int)width, CV_8UC4, (void*)baseAddress, stride);
	
	// Start tracking
	cv::Point3i currentPosition = m_trackingDelegate->startTracking(currentFrame);
	
	int rollError = currentPosition.x - (int)width/2;
	int roll = m_rollPIDCalc->run(rollError);
	
	int thrustError = currentPosition.y - (int)height/2;
	int thrust = m_thrustPIDCalc->run(thrustError);
	
	int pitchError = currentPosition.z - 68;
	//int pitchError = 100 - currentPosition.z;
	int pitch = m_pitchPIDCalc->run(pitchError);
	
	int yaw = 0;
	
	if(stopFlag)
		m_trafficController->sendParameter(0, 0, 0, 0);
	else
		m_trafficController->sendParameter(thrust, yaw, pitch, roll);

	// Redering preview
	[m_preview renderFromBuffer:imageBuffer];
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
	// Insert code here to initialize your application
	[m_strategyPopup addItemWithTitle:@"Static Tracking"];
	[m_strategyPopup addItemWithTitle:@"Dynamic Tracking"];
	[m_strategyPopup addItemWithTitle:@"Ball Tracking"];
	
	m_trackingDelegate = new TrackingDelegate();
	m_trackingDelegate->setStrategy(TrackingDelegate::kBall);
	m_trafficController = new TracfficController();
	m_thrustPIDCalc = new pid_calc_t(0, 0, 0, 0.03, 40000, -40000, 78000, 0);
	m_pitchPIDCalc = new pid_calc_t(0, 0, 0, 0.03, 30, -30, 30, -30);
	m_rollPIDCalc = new pid_calc_t(0, 0, 0, 0.03, 30, -30, 30, -30);
	m_yawPIDCalc = new pid_calc_t(0, 0, 0, 0);
	
	NSRect thrustFrame = NSMakeRect(1155, 650, 270, 180);
	[self addPIDGroupWithFrame:thrustFrame andCalObject:m_thrustPIDCalc];
	NSRect pitchFrame = NSMakeRect(1155, 498, 270, 180);
	[self addPIDGroupWithFrame:pitchFrame andCalObject:m_pitchPIDCalc];
	NSRect	rollFrame = NSMakeRect(1155, 348, 270, 180);
	[self addPIDGroupWithFrame:rollFrame andCalObject:m_rollPIDCalc];
	NSRect yawFrame = NSMakeRect(1155, 196, 270, 180);
	[self addPIDGroupWithFrame:yawFrame andCalObject:m_yawPIDCalc];
	
	m_camera = [[Camera alloc] init];
	m_camera.delegate = self;
	stopFlag = false;
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
	pid_calc_t* cal = (pid_calc_t*)[sender associatedObject];
	float value = [sender activeValue];
	switch(type)
	{
		case kPIDkp:
			cal->setKp(value);
			break;
		case kPIDki:
			cal->setKi(value);
			break;
		case kPIDkd:
			cal->setKd(value);
			break;
		case kPIDdt:
			cal->setDt(value);
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
		[m_stopButton setTitle:@"Stop"];
}

- (void)addPIDGroupWithFrame:(NSRect)freame andCalObject:(pid_calc_t*)obj
{
	m_controlWidget = [[[ControlWidgets alloc] initWithFrame:freame] autorelease];
	[m_controlWidget setTarget:self];
	[m_controlWidget setAction:@selector(pidValueChanged:)];
	m_controlWidget.associatedObject = (id)obj;
	[m_view addSubview:m_controlWidget];
}

@end
