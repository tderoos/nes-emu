//
//  nesView.h
//  nestex
//
//  Created by Tommy de Roos on 3/8/13.
//  Copyright (c) 2013 Tommy de Roos. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface nesView : NSOpenGLView
{
    // Timer object that can be attached to animationTimer method for time-based
    // animations
    NSTimer* timer;
    bool damage;

    unsigned int mFrameBuffer[256*256];
    unsigned int mTexture;
    unsigned char mButtonState;
}

@end


/*



#import <OpenGL/gl.h>
#import <Cocoa/Cocoa.h>


///////////////////////////////////////////////////////////////////////////////
// OpenGL error handling
///////////////////////////////////////////////////////////////////////////////
// error reporting as both window message and debugger string
void reportError (char * strError);
// if error dump gl errors to debugger string, return error
GLenum glReportError (void);

@interface BasicOpenGLView : NSOpenGLView
{
    // TODO: add object (pointer) for your OpenGL-based app
    
    // Timer object that can be attached to animationTimer method for time-based
    // animations
    NSTimer* timer;
    // Current time at construction
    CFAbsoluteTime start_time;
    // True only if a redraw is necessary, i.e. scene has changed
    bool damage;
    // This is a cheesy global variable that allows me to to call certain code
    // only once per OpenGL session, I'm not sure how necessary this is
    bool openGL_initialized;
}

///////////////////////////////////////////////////////////////////////////////
// File IO
///////////////////////////////////////////////////////////////////////////////
// Default open function, called when using File > Open dialog, but also when
// files (of correct type) are dragged to the dock icon
- (IBAction) openDocument: (id) sender;
// Helper method for openDocument that takes the file name of the given file
// directly so that it can be hooked up to the event of double clicking a file
// in Finder
// Inputs:
//   file_name  string containing path to file to be opened
// Returns:
//   true only on success
- (BOOL) openDocumentFromFileName: (NSString *)file_name;
// Default save function, called when using File > Save as ... dialog
- (IBAction) saveDocumentAs: (id) sender;


///////////////////////////////////////////////////////////////////////////////
// Mouse and Keyboard Input
///////////////////////////////////////////////////////////////////////////////
- (void) keyDown:(NSEvent *)theEvent;
- (void) keyUp:(NSEvent *)theEvent;
- (void) mouseDown:(NSEvent *)theEvent;
- (void) rightMouseDown:(NSEvent *)theEvent;
- (void) otherMouseDown:(NSEvent *)theEvent;
- (void) mouseUp:(NSEvent *)theEvent;
- (void) rightMouseUp:(NSEvent *)theEvent;
- (void) otherMouseUp:(NSEvent *)theEvent;
- (void) mouseMoved:(NSEvent *)theEvent;
- (void) mouseDragged:(NSEvent *)theEvent;
- (void) rightMouseDragged:(NSEvent *)theEvent;
- (void) otherMouseDragged:(NSEvent *)theEvent;
- (void) scrollWheel:(NSEvent *)theEvent;
- (void) viewDidMoveToWindow;
// OpenGL apps like to think of (0,0) being the top left corner, cocoa apps
// think of (0,0) as the bottom left corner. This simple flips the y coordinate
// according to the current height
// Inputs:
//   location  point of click according to Cocoa
// Returns
//   point of click according to with y coordinate flipped
- (NSPoint) flip_y:(NSPoint)location;


///////////////////////////////////////////////////////////////////////////////
// OpenGL
///////////////////////////////////////////////////////////////////////////////
// Called whenever openGL context changes size
- (void) reshape;
// Main display or draw function, called when redrawn
- (void) drawRect:(NSRect)rect;
// Set initial OpenGL state (current context is set)
// called after context is created
- (void) prepareOpenGL;
// this can be a troublesome call to do anything heavyweight, as it is called
// on window moves, resizes, and display config changes.  So be careful of
// doing too much here.  window resizes, moves and display changes (resize,
// depth and display config change)
- (void) update;


///////////////////////////////////////////////////////////////////////////////
// Animation
///////////////////////////////////////////////////////////////////////////////

// per-window timer function, basic time based animation preformed here
- (void) animationTimer:(NSTimer *)timer;
// Set an instance variable to the current time, so as to keep track of the time
// at which the app was started
- (void) setStartTime;
// Get the time since the start time: elapsed time since app was started
- (CFAbsoluteTime) getElapsedTime;


///////////////////////////////////////////////////////////////////////////////
// Cocoa
///////////////////////////////////////////////////////////////////////////////
- (BOOL) acceptsFirstResponder;
- (BOOL) becomeFirstResponder;
- (BOOL) resignFirstResponder;
- (void) awakeFromNib;
- (void) terminate:(NSNotification *)aNotification;

@end

*/