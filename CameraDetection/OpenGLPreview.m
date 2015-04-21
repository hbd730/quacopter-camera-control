//
//  OpenGLPreview.m
//  CameraDetection
//
//  Created by Peter Huang on 30/10/2014.
//  Copyright (c) 2014 FlightDynamics. All rights reserved.
//

#import "OpenGLPreview.h"
#include "Debug.h"

@implementation OpenGLPreview

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    return self;
}

- (void)putBufferInTexture:(CVPixelBufferRef)pixBuf
{
	void*	address;
	size_t	rowBytes;
	size_t	frameWidth, frameHeight;
	bool	newTexture = false;
	int		type;
	GLint internalFormat = GL_RGBA;
	GLint format = GL_BGRA;
	
	[[self openGLContext] makeCurrentContext];
	BAIL_IF(CVPixelBufferLockBaseAddress(pixBuf, 0), "CVPixelBufferLockBaseAddress failed.\n");
	
	address     = CVPixelBufferGetBaseAddress(pixBuf);
	rowBytes    = CVPixelBufferGetBytesPerRow(pixBuf);
	frameWidth  = CVPixelBufferGetWidth(pixBuf);
	frameHeight = CVPixelBufferGetHeight(pixBuf);
	
	// If this frame's dimensions differ to those of the existing texture,
	// create a new texture object.
	if (((frameWidth != m_textureWidth) || (frameHeight != m_textureHeight)) && (m_textureId != 0))
	{
		glDeleteTextures(1, &m_textureId);
		m_textureId = 0;
	}
	
	if (! m_textureId)
	{
		glGenTextures(1, &m_textureId);
		
		m_textureWidth = (GLint)frameWidth;
		m_textureHeight = (GLint)frameHeight;
		newTexture = true;
	}
	
	glBindTexture(GL_TEXTURE_RECTANGLE_EXT, m_textureId);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, (GLint)frameWidth);
	
	glDisable(GL_FRAGMENT_PROGRAM_ARB);
	
	type = GL_UNSIGNED_BYTE;
	
	if(rowBytes != frameWidth)
	{
		internalFormat = GL_RGBA;
		format = GL_BGRA;
	}
	else
	{
		internalFormat = GL_DEPTH_COMPONENT;
		format = GL_DEPTH_COMPONENT;
	}
	if (newTexture)
	{
		// Use glTexImage2D() since this is a new texture.
		glTexImage2D(GL_TEXTURE_RECTANGLE_EXT, 0, internalFormat, m_textureWidth, m_textureHeight, 0, format, type, address);
	}
	else
	{
		// Use glTexSubImageID() since this is an existing texture.
		glTexSubImage2D(GL_TEXTURE_RECTANGLE_EXT, 0, 0, 0, m_textureWidth, m_textureHeight, format, type, address);
	}
	
	BAIL_IF(CVPixelBufferUnlockBaseAddress(pixBuf, 0), "CVPixelBufferUnlockBaseAddress failed.\n");
bail:
	return;
}

- (void)reshape
{
	m_needsReshape = YES;
}

- (void) render
{
	// OpenGL code goes here
	if (m_needsReshape)
	{
		NSRect  frame = [self frame];
		
		[self update];
		
		if(NSIsEmptyRect([self visibleRect]))
		{
			glViewport(0, 0, 1, 1);
		}
		else
		{
			glViewport(0, 0, (GLsizei)frame.size.width, (GLsizei)frame.size.height);
		}
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		
		m_needsReshape = NO;
	}
	
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	
	// Render frame
	// Draw the entire texture image
	glColor3f(1.0f, 1.0f, 1.0f);
	glEnable(GL_TEXTURE_RECTANGLE_EXT);
	
	glBindTexture(GL_TEXTURE_RECTANGLE_EXT, m_textureId);
	
	glBegin(GL_QUADS);
	glTexCoord2i(0, m_textureHeight);
	glVertex2f(-1, -1);
	glTexCoord2i(m_textureWidth, m_textureHeight);
	glVertex2f(1, -1);
	glTexCoord2i(m_textureWidth, 0);
	glVertex2f(1, 1);
	glTexCoord2i(0,	0);
	glVertex2f(-1, 1);
	glEnd();
	
	glDisable(GL_TEXTURE_RECTANGLE_EXT);
	
	glFlush();
}

- (void)drawView
{
	[[self openGLContext] makeCurrentContext];
	CGLLockContext(CGLGetCurrentContext());
	[self render];
	CGLFlushDrawable(CGLGetCurrentContext());
	CGLUnlockContext(CGLGetCurrentContext());
}

- (CVReturn)renderFromBuffer:(CVPixelBufferRef)pixBuf
{
	// There is no autorelease pool when this method is called
	// because it will be called from a background thread.
	// It's important to create one or app can leak objects.
	if (pixBuf)
	{
		NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
		[self putBufferInTexture:pixBuf];
		[self drawView];
		[pool release];
		return kCVReturnSuccess;
	}
	else
		return kCVReturnInvalidArgument;
}

- (void)prepareOpenGL
{
	[super prepareOpenGL];
	GLint swapInterval = 1;
	[[self openGLContext] setValues:&swapInterval forParameter:NSOpenGLCPSwapInterval];
}

@end


@implementation TrackingPreview

@synthesize m_viewListener;

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    return self;
}

- (void)mouseDown:(NSEvent *)theEvent
{
	if(m_viewListener)
		[m_viewListener viewEventHandler:theEvent];
    [[self nextResponder] mouseDown:theEvent];
}

- (void)mouseUp:(NSEvent *)theEvent
{
	if(m_viewListener)
		[m_viewListener viewEventHandler:theEvent];
    [[self nextResponder] mouseDown:theEvent];
}

- (NSPoint)transform:(NSPoint)clickedPoint
{
	NSPoint point;
	float x = [self convertPoint: clickedPoint fromView: nil].x;  // from physical coorinate to NSView coorindate
	float y = [self convertPoint: clickedPoint fromView: nil].y;
	point.x = x * m_textureWidth / [self frame].size.width;     // from NSView coorindate to cv::Mat coordinate
	point.y = m_textureHeight - y * m_textureHeight / [self frame].size.height;
	return point;
}

@end

@implementation ImageProcessingPreview

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    return self;
}

@end
