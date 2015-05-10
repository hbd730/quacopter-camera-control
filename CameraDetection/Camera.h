//
//  Camera.h
//  CameraDetection
//
//  Created by Peter Huang on 21/10/2014.
//  Copyright (c) 2014 Peter Huang. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreMedia/CoreMedia.h>
#import <AVFoundation/AVFoundation.h>

@protocol CameraDelegate <NSObject>

- (void) frameCaptured:(CVImageBufferRef) imageBuffer;

@end

@interface Camera: NSObject<AVCaptureVideoDataOutputSampleBufferDelegate>

@property (assign) id<CameraDelegate> delegate;

- (bool) hasMultipleCameras;
- (void) startRunning;
- (void) stopRunning;

@end
