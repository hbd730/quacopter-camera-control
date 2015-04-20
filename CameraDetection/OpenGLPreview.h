//
//  OpenGLPreview.h
//  CameraDetection
//
//  Created by Peter Huang on 30/10/2014.
//  Copyright (c) 2014 FlightDynamics. All rights reserved.
//

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


@protocol ViewListener <NSObject>

- (void)viewEventHandler:(NSEvent*)theEvent; // register a viewEvent handler

@end

// Tracking preview handle mouse event such as region and PID setpoint selection
@interface TrackingPreview : OpenGLPreview
{
	
}

@property (assign) id<ViewListener> m_viewListener;
- (NSPoint)transform:(NSPoint)clickedPoint;

@end

// ImageProcessing renders debugging image
@interface ImageProcessingPreview : OpenGLPreview
{
	
}

@end