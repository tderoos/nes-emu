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


class nestor
{
public:
                nestor(const char* inRom);

    void        RunToVBlank(char inButtonState, uint32_t* ioFrameBuffer);
    
private:
    Rom         mRom;
    Ram         mRam;
    PPU2C07     mPPU;
    IO          mIO;
    CPU6502     mCPU;
    uint32_t    mClock;
};

#endif /* defined(__nestor__nestor__) */
