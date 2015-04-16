//
//  OpenGLPreview.h
//  CameraDetection
//
//  Created by Peter Huang on 30/10/2014.
//  Copyright (c) 2014 FlightDynamics. All rights reserved.
//

#include "Tracking.h"
#import <Cocoa/Cocoa.h>
#import <CoreVideo/CoreVideo.h>
#import <OpenGL/OpenGL.h>
#import <OpenGL/gl.h>

// OpenGLPreview takes CVPixelBufferRef as input and render on the view
@interface OpenGLPreview : NSOpenGLView
{
	GLuint				m_textureId;
	GLint				m_textureWidth;
	GLint				m_textureHeight;
	BOOL				m_needsReshape;
}

- (void)reshape;
- (void)render;
- (void)drawView;
- (void)prepareOpenGL;
- (void)putBufferInTexture:(CVPixelBufferRef)pixBuf;
- (CVReturn) renderFromBuffer:(CVPixelBufferRef)pixBuf;

@end

// Tracking preview handle mouse event such as region and PID setpoint selection
@interface TrackingPreview : OpenGLPreview
{
	ITracking*	m_viewlistener;
	NSPoint m_clickedPoint;
	SEL callback;
}

- (void)setViewListener:(ITracking*)listener;
- (NSPoint)transform:(NSPoint)clickedPoint;
- (cv::Point2i)getClickedPoint;
- (void)setCallBack;

@end

// ImageProcessing renders debugging image
@interface ImageProcessingPreview : OpenGLPreview
{
	
}

@end