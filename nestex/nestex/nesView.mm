//
//  nesView.m
//  nestex
//
//  Created by Tommy de Roos on 3/8/13.
//  Copyright (c) 2013 Tommy de Roos. All rights reserved.
//

#import "nesView.h"

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
    glClearColor(0.5f, 0.5f, 0.5f, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
    [[NSRunLoop currentRunLoop] addTimer:timer
                                 forMode:NSEventTrackingRunLoopMode];
}


- (void)animationTimer:(NSTimer *)timer
{
    // TODO: handle timer based redraw (animation) here
    bool your_app_says_to_redraw = true;
    if(your_app_says_to_redraw || damage)
    {
        damage = false;
        [self drawRect:[self bounds]];
    }
}



- (void) update
{
    [super update];
}

@end




/*


 
 #import "BasicOpenGLView.h"
 // For functions like gluErrorString()
 #import <OpenGL/glu.h>
 #ifdef __APPLE__
 #define _MACOSX
 #endif
 
 
 void reportError (char * strError)
 {
 // Set up a fancy font/display for error messages
 NSMutableDictionary *attribs = [NSMutableDictionary dictionary];
 [attribs setObject: [NSFont fontWithName: @"Monaco" size: 9.0f]
 forKey: NSFontAttributeName];
 [attribs setObject: [NSColor whiteColor]
 forKey: NSForegroundColorAttributeName];
 // Build the error message string
 NSString * errString = [NSString stringWithFormat:@"Error: %s.", strError];
 // Display to log
 NSLog (@"%@\n", errString);
 }
 
 GLenum glReportError (void)
 {
 // Get current OpenGL error flag
 GLenum err = glGetError();
 // If there's an error report it
 if (GL_NO_ERROR != err)
 {
 reportError ((char *) gluErrorString (err));
 }
 return err;
 }
 
 @implementation BasicOpenGLView
 
 -(IBAction) openDocument: (id) sender
 {
 NSOpenPanel *tvarNSOpenPanelObj  = [NSOpenPanel openPanel];
 // TODO: Add a item to this list corresponding to each file type extension
 // this app supports opening
 // Create an array of strings specifying valid extensions and HFS file types.
 NSArray *fileTypes = [NSArray arrayWithObjects:
 @"obj",
 @"OBJ",
 NSFileTypeForHFSTypeCode('TEXT'),
 nil];
 // Create an Open file... dialog
 NSInteger tvarNSInteger = [tvarNSOpenPanelObj runModalForTypes:fileTypes];
 // If the user selected OK then load the file
 if(tvarNSInteger == NSOKButton)
 {
 // Pass on file name to opener helper
 [self openDocumentFromFileName:[tvarNSOpenPanelObj filename]];
 }
 }
 

 - (NSPoint) flip_y:(NSPoint) location
 {
 // Get openGL context size
 NSRect rectView = [self bounds];
 // Cocoa gives opposite of OpenGL y direction, flip y direction
 location.y = rectView.size.height - location.y;
 return location;
 }
 
 - (void) reshape
 {
 NSRect rectView = [self bounds];
 // TODO: Handle resize window using the following
 NSLog(@"New context size: %g %g\n",
 rectView.size.width,rectView.size.height);
 }
 

 
 - (void)animationTimer:(NSTimer *)timer
 {
 // TODO: handle timer based redraw (animation) here
 bool your_app_says_to_redraw = true;
 if(your_app_says_to_redraw || damage)
 {
 damage = false;
 [self drawRect:[self bounds]];
 }
 }
 
 - (void) setStartTime
 {
 start_time = CFAbsoluteTimeGetCurrent ();
 }
 
 - (CFAbsoluteTime) getElapsedTime
 {
 return CFAbsoluteTimeGetCurrent () - start_time;
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
 

 
 - (void) terminate:(NSNotification *)aNotification
 {
 // TODO: delete your app's object
 NSLog(@"Terminating");
 }
 
 @end

*/