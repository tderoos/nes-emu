//
//  nesView.m
//  nestex
//
//  Created by Tommy de Roos on 3/8/13.
//  Copyright (c) 2013 Tommy de Roos. All rights reserved.
//

#import "nesView.h"
#include "../nestor/nestor.h"
#include <OpenGL/gl.h>

@implementation nesView

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
    const GLint swapInt = 1;
    // set to vbl sync
    [[self openGLContext] setValues:&swapInt
                       forParameter:NSOpenGLCPSwapInterval];
    
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &mTexture);
    glBindTexture(GL_TEXTURE_2D, mTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); //set its parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    
    NSLog(@"prepareOpenGL\n");
}


- (void) drawRect:(NSRect)rect
{
    // TODO: handle draw event
    // For now just clear the screen with a time dependent color
    
    glClear (GL_COLOR_BUFFER_BIT);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glBindTexture(GL_TEXTURE_2D, mTexture); //bind the first texture.
    
    glBegin(GL_QUADS);
        glTexCoord2i(0,0); glVertex2i(-1,1);
        glTexCoord2i(0,1); glVertex2i(-1,-1);
        glTexCoord2i(1,1); glVertex2i(1,-1);
        glTexCoord2i(1,0); glVertex2i(1,1);
    glEnd();
    
    // Flush all OpenGL calls
    glFlush();
    // Flush OpenGL context
    [[self openGLContext] flushBuffer];
}

-(void) render:(unsigned int*)inBuffer
{
    glBindTexture(GL_TEXTURE_2D, mTexture); //bind the first texture.
    //then load it into the graphics hardware:
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, inBuffer );
    
    [self drawRect:[self bounds]];
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
