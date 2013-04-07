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

    unsigned int mTexture;
    unsigned char mButtonState;
}

- (unsigned char)getButtonState;
-(void) render:(unsigned int*)inBuffer;

@end
