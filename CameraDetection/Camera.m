//
//  Camera.m
//  CameraDetection
//
//  Created by Peter Huang on 21/10/2014.
//  Copyright (c) 2014 FlightDynamics. All rights reserved.
//

#import "Camera.h"

@interface Camera ()
{
	AVCaptureSession * session;
	NSArray * captureDevices;
	AVCaptureDeviceInput * captureInput;
	AVCaptureVideoDataOutput * captureOutput;
	int currentCameraIndex;
}
@end

@implementation Camera
@synthesize delegate;

- (id) init
{
	if (self = [super init])
	{
		currentCameraIndex = 0;
		session = [[AVCaptureSession alloc] init];
		[session setSessionPreset:AVCaptureSessionPreset1280x720];
		captureDevices = [AVCaptureDevice devicesWithMediaType:AVMediaTypeVideo];
		
		AVCaptureDevice *videoDevice = [captureDevices objectAtIndex:0];
		
		NSError * error;
		captureInput = [AVCaptureDeviceInput deviceInputWithDevice:videoDevice error:&error];
		
		if (error)
		{
			NSLog(@"Couldn't create video input");
		}
        
		captureOutput = [[AVCaptureVideoDataOutput alloc] init];
		captureOutput.alwaysDiscardsLateVideoFrames = YES;
		
		// Set the video output to store frame in BGRA (It is supposed to be faster)
		NSString* key = (NSString*)kCVPixelBufferPixelFormatTypeKey;
		NSNumber* value = [NSNumber numberWithUnsignedInt:kCVPixelFormatType_32BGRA];
		NSDictionary* videoSettings = [NSDictionary dictionaryWithObject:value forKey:key];
		[captureOutput setVideoSettings:videoSettings];
		
		/*We create a serial queue to handle the processing of our frames*/
		dispatch_queue_t queue;
		queue = dispatch_queue_create("com.computer-vision-talks.cameraQueue", NULL);
		[captureOutput setSampleBufferDelegate:self queue:queue];
		
		[session addInput:captureInput];
		[session addOutput:captureOutput];
	}

	return self;
}

- (bool) hasMultipleCameras
{
	return [captureDevices count] > 1;
}

- (void) startRunning
{
	[session startRunning];
}

- (void) stopRunning
{
	[session stopRunning];
}

- (void) dealloc
{
	[session release];
	[captureOutput release];
	[super dealloc];
}

- (void)captureOutput:(AVCaptureOutput *)captureOutput didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer fromConnection:(AVCaptureConnection *)connection
{
	if (!delegate)
		return;
	
	CVImageBufferRef imageBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);
	
	/*Lock the image buffer*/
	CVPixelBufferLockBaseAddress(imageBuffer,0);
	
	/*Get information about the image*/
	[delegate frameCaptured:imageBuffer];
	
	/*We unlock the  image buffer*/
	CVPixelBufferUnlockBaseAddress(imageBuffer,0);
}

@end


