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

    void        RunToVBlank(char inButtonState, uint32_t* ioFrameBuffer);
    
private:
    Rom         mRom;
    Ram         mRam;
    PPU2C07     mPPU;
    APU         mAPU;
    IO          mIO;
    CPU6502     mCPU;
    
    UInt32      mClockToCPU;
    UInt32      mClockToPPU;
    UInt32      mClockToAPU;
    
};

#endif /* defined(__nestor__nestor__) */
