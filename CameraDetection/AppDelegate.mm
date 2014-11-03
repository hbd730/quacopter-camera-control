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

@interface AppDelegate ()<CameraDelegate>
{
	TrackingDelegate* m_trackingDelegate;
}

- (IBAction)strategyChanged:(id)sender;
@property (nonatomic, strong) Camera* m_camera;
@property (assign) IBOutlet OpenGLPreview* m_preview;
@property (assign) IBOutlet NSPopUpButton* m_strategyPopup;

@end

@implementation AppDelegate
@synthesize m_camera;
@synthesize m_preview;
@synthesize m_strategyPopup;

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
	m_trackingDelegate->startTracking(currentFrame);
	
	// Redering preview
	[m_preview renderFromBuffer:imageBuffer];
}

- (IBAction)strategyChanged:(id)sender
{
	TrackingDelegate::StrategyType type;
	type = (TrackingDelegate::StrategyType)[m_strategyPopup indexOfSelectedItem];
	m_trackingDelegate->setStrategy(type);
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
	// Insert code here to initialize your application
	[m_strategyPopup addItemWithTitle:@"Static Tracking"];
	[m_strategyPopup addItemWithTitle:@"Dynamic Tracking"];
	[m_strategyPopup addItemWithTitle:@"Ball Tracking"];
	m_trackingDelegate = new TrackingDelegate();
	m_trackingDelegate->setStrategy(TrackingDelegate::kStatic);
	m_camera = [[Camera alloc] init];
	m_camera.delegate = self;
	[m_camera startRunning];
}

- (void)applicationWillTerminate:(NSNotification *)aNotification
{
	// Insert code here to tear down your application
	delete m_trackingDelegate;
	[m_camera stopRunning];
	[m_camera dealloc];
}

@end
