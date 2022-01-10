//
//  nesView.m
//  nestex
//
//  Created by Tommy de Roos on 3/8/13.
//  Copyright (c) 2013 Tommy de Roos. All rights reserved.
//

#import "nesView.h"
#include "../nestor/nestor.h"
#include "../nestor/Profiler.h"
#include <OpenGL/gl.h>

@implementation nesView

//ProfileBar pb("SwapInterval");

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code here.
    }
    
    return self;
}

- (void) prepareOpenGL
{
    [super prepareOpenGL];

    const GLint swapInt = 0;
    // set to vbl sync
    [[self openGLContext] setValues:&swapInt
                       forParameter:NSOpenGLCPSwapInterval];
    
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    glEnable(GL_TEXTURE_2D);
    glGenTextures(2, mTexture);
    glBindTexture(GL_TEXTURE_2D, mTexture[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); //set its parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, mTexture[1]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); //set its parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    
    mCurTexture = 0;
    
//    pb.Tick();
    
    // Create a display link capable of being used with all active displays
//    CVDisplayLinkCreateWithActiveCGDisplays(&m_displayLink);
//    
//    // Set the renderer output callback function
//    CVDisplayLinkSetOutputCallback(m_displayLink, &MyDisplayLinkCallback, (__bridge void*)self);
    
    NSLog(@"prepareOpenGL\n");
}


- (void) drawRect:(NSRect)rect
{
    [[self openGLContext] makeCurrentContext];
    
    // Draw a single quad with our frame
    glBegin(GL_QUADS);
        glTexCoord2i(0,0); glVertex2i(-1,1);
        glTexCoord2i(0,1); glVertex2i(-1,-1);
        glTexCoord2i(1,1); glVertex2i(1,-1);
        glTexCoord2i(1,0); glVertex2i(1,1);
    glEnd();
    
    // Flush OpenGL context
    [[self openGLContext] flushBuffer];
//    pb.Tick();
}

-(void) render:(unsigned int*)inBuffer
{
    [[self openGLContext] makeCurrentContext];

    glBindTexture(GL_TEXTURE_2D, mTexture[mCurTexture]);
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, inBuffer );

//    [self drawRect:[self bounds]];
    [self setNeedsDisplay:YES];
    mCurTexture = 1-mCurTexture;
}


- (BOOL)acceptsFirstResponder
{
    return YES;
}

- (BOOL)becomeFirstResponder
{
    return  YES;
}

- (BOOL)resignFirstResponder
{
    return YES;
}

- (BOOL)isReleasedWhenClosed
{
    return YES;
}


//
// Read # |    1      2      3      4      5      6      7      8
// -------+---------------------------------------------------------
// Button |    A      B   SELECT   START   UP    DOWN   LEFT  RIGHT
//
-(void)keyUp:(NSEvent*)event
{
    int bit = 0;
    switch( [event keyCode] ) {
        case 40: bit = 1 << 0; break;
        case 37: bit = 1 << 1; break;
        case 5:  bit = 1 << 2; break;
        case 4:  bit = 1 << 3; break;
        case 13: bit = 1 << 4; break;
        case 1:  bit = 1 << 5; break;
        case 0:  bit = 1 << 6; break;
        case 2:  bit = 1 << 7; break;
    }
    mButtonState = mButtonState & ~bit;
}


-(void)keyDown:(NSEvent*)event
{
    int bit = 0;
    switch( [event keyCode] ) {
        case 40: bit = 1 << 0; break;
        case 37: bit = 1 << 1; break;
        case 5:  bit = 1 << 2; break;
        case 4:  bit = 1 << 3; break;
        case 13: bit = 1 << 4; break;
        case 1:  bit = 1 << 5; break;
        case 0:  bit = 1 << 6; break;
        case 2:  bit = 1 << 7; break;
    }
    mButtonState = mButtonState | bit;
}

- (unsigned char)getButtonState
{
    return mButtonState;
}

@end
