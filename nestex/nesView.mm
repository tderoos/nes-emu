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

nestor* nes = NULL;

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
    
/*    if(!openGL_initialized)
    {
        // Get command line arguments and find whether stealFocus is set to YES
        NSUserDefaults *args = [NSUserDefaults standardUserDefaults];
        // also find out if app should steal focus
        bool stealFocus = [args boolForKey:@"stealFocus"];
        if(stealFocus)
        {
            // Steal focus means that the apps window will appear in front of all
            // other programs when it launches even in front of the calling
            // application (e.g. a terminal)
            [[NSApplication sharedApplication] activateIgnoringOtherApps:YES];
        }
        // TODO: Initialize OpenGL app, do anything here that you need to *after*
        // the OpenGL context is initialized (load textures, shaders, etc.)
        openGL_initialized = true;
    }*/
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
    //then load it into the graphics hardware:
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, mFrameBuffer );
    
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

- (void) awakeFromNib
{
//    openGL_initialized = false;
    // keep track of start/launch time
//    [self setStartTime];
    // start animation timer
    timer = [NSTimer timerWithTimeInterval:(1.0f/60.0f) target:self
                                  selector:@selector(animationTimer:) userInfo:nil repeats:YES];
    [[NSRunLoop currentRunLoop] addTimer:timer forMode:NSDefaultRunLoopMode];

    // ensure timer fires during resize
    [[NSRunLoop currentRunLoop] addTimer:timer forMode:NSEventTrackingRunLoopMode];


/// PPU
//    nes = new nestor("/Users/tderoos/Projects/playground/nestor/sprite_ram.nes");       // PASSED ($1)
//    nes = new nestor("/Users/tderoos/Projects/playground/nestor/vram_access.nes");      // PASSED ($1)
//    nes = new nestor("/Users/tderoos/Projects/playground/nestor/01-vbl_basics.nes");      // PASSED
//    nes = new nestor("/Users/tderoos/Projects/playground/nestor/02-vbl_set_time.nes");      // PASSED
//    nes = new nestor("/Users/tderoos/Projects/playground/nestor/ppu_vbl_nmi.nes");
//    nes = new nestor("/Users/tderoos/Projects/playground/nestor/04-nmi_control.nes");
//    nes = new nestor("/Users/tderoos/Projects/playground/nestor/05-nmi_timing.nes");
//    nes = new nestor("/Users/tderoos/Projects/playground/nestor/06-suppression.nes");
//    nes = new nestor("/Users/tderoos/Projects/playground/nestor/09-even_odd_frames.nes");     // PASSED
//    nes = new nestor("/Users/tderoos/Projects/playground/nestor/10-even_odd_timing.nes");
//    nes = new nestor("/Users/tderoos/Projects/playground/nestor/scrolltest_scroll.nes");
//    nes = new nestor("/Users/tderoos/Projects/playground/nestor/s0.nes");

//    nes = new nestor("/Users/tderoos/Projects/playground/nestor/vbl_clear_time.nes");       // BROKEN TEST
    
// APU
//    nes = new nestor("/Users/tderoos/Projects/playground/nestor/apu_test.nes");
//    nes = new nestor("/Users/tderoos/Projects/playground/nestor/05.len_timing_mode0.nes");    // PASSED
//    nes = new nestor("/Users/tderoos/Projects/playground/nestor/6-irq_flag_timing.nes");
//    nes = new nestor("/Users/tderoos/Projects/playground/nestor/5-len_timing.nes");     // PASSED
    
// CPU
//    nes = new nestor("/Users/tderoos/Projects/playground/nestor/nestest.nes");        // PASSED
//    nes = new nestor("/Users/tderoos/Projects/playground/nestor/instr_timing.nes");
//    nes = new nestor("/Users/tderoos/Projects/playground/nestor/all_instrs.nes");
//    nes = new nestor("/Users/tderoos/Projects/playground/nestor/cpu_timing_test.nes");  // PASSED
//    nes = new nestor("/Users/tderoos/Projects/playground/nestor/registers.nes");
    
// Working games
//    nes = new nestor("/Users/tderoos/Projects/playground/nestor/smb1.nes");
//    nes = new nestor("/Users/tderoos/Projects/playground/nestor/DEFENDR2.NES");
//    nes = new nestor("/Users/tderoos/Projects/playground/nestor/Kid Icarus.nes");
//    nes = new nestor("/Users/tderoos/Projects/playground/nestor/Metroid.nes");
//    nes = new nestor("/Users/tderoos/Projects/playground/nestor/Blaster Master.nes");
    nes = new nestor("/Users/tderoos/Projects/playground/nestor/Legend of Zelda.nes");

// Broken games

// Unsupported (mapper)
//    nes = new nestor("/Users/tderoos/Projects/playground/nestor/Super Off-Road (E) [!].nes");

    mButtonState = 0;
}

- (void)animationTimer:(NSTimer *)timer
{
    // TODO: handle timer based redraw (animation) here
    bool your_app_says_to_redraw = true;
    if(your_app_says_to_redraw || damage)
    {
        nes->RunToVBlank(mButtonState, mFrameBuffer);
        
        damage = false;
        [self drawRect:[self bounds]];
    }
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

+ (void)onTerminate
{
    nes->onExit();
    delete nes;
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

- (void) update
{
    [super update];
}

@end
