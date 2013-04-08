//
//  AppDelegate.m
//  nestex
//
//  Created by Tommy de Roos on 3/8/13.
//  Copyright (c) 2013 Tommy de Roos. All rights reserved.
//

#import "AppDelegate.h"
#import "nesView.h"
#import "nesAudio.h"
#include "../nestor/nestor.h"


@implementation AppDelegate

- (void)dealloc
{
    [super dealloc];
}

- (void)initNestor
{
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
//    nes = new nestor("/Users/tderoos/Projects/playground/nestor/5-len_timing.nes");       // PASSED
//    nes = new nestor("/Users/tderoos/Projects/playground/nestor/Roms/APU/01.len_ctr.nes");    // PASSED
//    nes = new nestor("/Users/tderoos/Projects/playground/nestor/Roms/APU/02.len_table.nes");  // PASSED
//    nes = new nestor("/Users/tderoos/Projects/playground/nestor/Roms/APU/03.irq_flag.nes");   // PASSED
//    nes = new nestor("/Users/tderoos/Projects/playground/nestor/Roms/APU/08.irq_timing.nes");
//    nes = new nestor("/Users/tderoos/Projects/playground/nestor/Roms/APU/SNDTEST.nes");

    
// CPU
//    nes = new nestor("/Users/tderoos/Projects/playground/nestor/nestest.nes");        // PASSED
//    nes = new nestor("/Users/tderoos/Projects/playground/nestor/instr_timing.nes");
//    nes = new nestor("/Users/tderoos/Projects/playground/nestor/all_instrs.nes");
//    nes = new nestor("/Users/tderoos/Projects/playground/nestor/cpu_timing_test.nes");  // PASSED
//    nes = new nestor("/Users/tderoos/Projects/playground/nestor/registers.nes");

// Working games
    nes = new nestor("/Users/tderoos/Projects/playground/nestor/smb1.nes");
//    nes = new nestor("/Users/tderoos/Projects/playground/nestor/DEFENDR2.NES");
//    nes = new nestor("/Users/tderoos/Projects/playground/nestor/Kid Icarus.nes");
//    nes = new nestor("/Users/tderoos/Projects/playground/nestor/Metroid.nes");
//    nes = new nestor("/Users/tderoos/Projects/playground/nestor/Blaster Master.nes");
//    nes = new nestor("/Users/tderoos/Projects/playground/nestor/Legend of Zelda.nes");

// Broken games

// Unsupported (mapper)
//    nes = new nestor("/Users/tderoos/Projects/playground/nestor/Super Off-Road (E) [!].nes");
    
}


- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    // Insert code here to initialize your application
    [self initNestor];
    
    // Start update timer
    timer = [NSTimer timerWithTimeInterval:(1.0f/240.0f) target:self
                                  selector:@selector(nesFrame:) userInfo:nil repeats:YES];
    [[NSRunLoop currentRunLoop] addTimer:timer forMode:NSDefaultRunLoopMode];
    
    mButtonState = 0;
    audio = [[nesAudio alloc] init];
    [audio render:mAudioBuffer bufferSize:735];
}

- (void)nesFrame:(NSTimer *)timer
{
    if ([audio needsRefresh] && nes != NULL)
    {
        mButtonState = [_view getButtonState];
        nes->RunToVBlank(mButtonState, mFrameBuffer, mAudioBuffer);
        [audio render:mAudioBuffer bufferSize:735];
        [_view render:mFrameBuffer];
    }
}

- (void)applicationWillTerminate:(NSNotification *)aNotification
{
    // Insert code here to terminate your application
    if (nes != NULL)
    {
        nes->onExit();
        delete nes;
        nes = NULL;
    }
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)theApplication
{
	return YES;
}

@end
