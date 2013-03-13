//
//  nestor.cpp
//  nestor
//
//  Created by Tommy de Roos on 3/8/13.
//
//

#include "nestor.h"


nestor::nestor(const char* inRom) :
    mRom(inRom),
    mRam(0x8000),
    mPPU(&mRom),
    mIO(&mRam, &mRom, &mPPU),
    mCPU(&mIO),
    mClock(0)
{
}


void nestor::RunToVBlank(char inButtonState, uint32_t* ioFrameBuffer)
{
    bool vblank = false;
    
    mIO.SetButtonState(inButtonState);
    
    int cycles = 0;
    
    do
    {
        if (mClock > 113)
        {
            mPPU.Scanline(ioFrameBuffer);
            mIO.SetNMI(mPPU.GetNMI());
            vblank = mPPU.GetScanline() == 241;
            mClock -= 113;
        }
        
        mCPU.Tick();
        mIO.Tick();

        mClock++;
        cycles++;
    }
    while (!vblank);
    
    cycles = cycles;
}