//
//  nestor.h
//  nestor
//
//  Created by Tommy de Roos on 3/8/13.
//
//

#ifndef __nestor__nestor__
#define __nestor__nestor__

//#include <iostream>

#include "Rom.h"
#include "Ram.h"
#include "IO.h"
#include "CPU6502.h"
#include "PPU2C07.h"
#include "APU.h"


class nestor
{
public:
                    nestor(const char* inRom);
    void            onExit();

    void            RunToVBlank(char inButtonState, uint32* ioFrameBuffer, uint8* ioAudioBuffer);
    const uint32*   GetFrameBuffer() const          { return mFrameBuffer; }
    
private:
    uint32      mFrameBuffer[256*256];
    
    Rom         mRom;
    Ram         mRam;
    PPU2C07     mPPU;
    APU         mAPU;
    IO          mIO;
    CPU6502     mCPU;
};

#endif /* defined(__nestor__nestor__) */
