//
//  AppDelegate.m
//  nestex
//
//  Created by Tommy de Roos on 3/8/13.
//  Copyright (c) 2013 Tommy de Roos. All rights reserved.
//

#import "AppDelegate.h"
#import "nesView.h"

@implementation AppDelegate

- (void)dealloc
{
    [super dealloc];
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    // Insert code here to initialize your application
}

- (void)applicationWillTerminate:(NSNotification *)aNotification
{
    // Insert code here to terminate your application
    [nesView onTerminate];
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)theApplication
{
	return YES;
}

@end
