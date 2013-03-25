//
//  nestor.cpp
//  nestor
//
//  Created by Tommy de Roos on 3/8/13.
//
//

#include "nestor.h"


const UInt32 kCPUUpdateClock   = 0;
const UInt32 kPPUScanlineClock = 113;
const UInt32 kAPUUpdateClock   = 7458;


nestor::nestor(const char* inRom) :
    mRom(inRom),
    mRam(0x8000),
    mPPU(&mRom),
    mIO(&mRam, &mRom, &mPPU, &mAPU),
    mCPU(&mIO),
    mClockToCPU(0),
    mClockToPPU(kPPUScanlineClock),
    mClockToAPU(kAPUUpdateClock)
{
}


void nestor::RunToVBlank(char inButtonState, uint32_t* ioFrameBuffer)
{
    bool vblank = false;
    
    mIO.SetButtonState(inButtonState);
    
    int cycles = 0;
    
    do
    {
        if (mClockToPPU-- == 0)
        {
            mPPU.Scanline(ioFrameBuffer);
            mIO.SetNMI(mPPU.GetNMI());
            vblank = mPPU.GetScanline() == 241;
            mClockToPPU = kPPUScanlineClock;
        }
        
        if (mClockToAPU-- == 0)
        {
            mAPU.Update();
//            mIO.SetIRQ(mAPU.GetInterrupt());
            mClockToAPU = kAPUUpdateClock;
        }

        if (mClockToCPU-- == 0)
        {
            mCPU.Tick();
            mClockToCPU = kCPUUpdateClock;
        }
        
        mIO.Tick();

        cycles++;
    }
    while (!vblank);
    
    cycles = cycles;
}

//7458.333333333333333