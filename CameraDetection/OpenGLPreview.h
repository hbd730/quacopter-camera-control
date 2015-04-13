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

class ITracking;

@interface OpenGLPreview : NSOpenGLView
{
	GLuint				m_textureId;
	GLint				m_textureWidth;
	GLint				m_textureHeight;
	BOOL				m_needsReshape;
	ITracking*			m_viewlistener;
}

- (void)reshape;
- (void)render;
- (void)drawView;
- (void)prepareOpenGL;
- (void)putBufferInTexture:(CVPixelBufferRef)pixBuf;
- (CVReturn) renderFromBuffer:(CVPixelBufferRef)pixBuf;
- (void) setViewListener:(ITracking*)listener;
@end
