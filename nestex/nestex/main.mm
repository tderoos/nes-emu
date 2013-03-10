//
//  main.m
//  nestex
//
//  Created by Tommy de Roos on 3/8/13.
//  Copyright (c) 2013 Tommy de Roos. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#include "../../nestor/nestor.h"

int main(int argc, char *argv[])
{
    nestor nes("/Users/tderoos/Projects/nestor/nestest.nes");
    
    return NSApplicationMain(argc, (const char **)argv);
}
