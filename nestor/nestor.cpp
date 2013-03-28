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
    mIO(&mRam, &mRom, &mPPU, &mAPU),
    mCPU(&mIO)
{
}


void nestor::RunToVBlank(char inButtonState, uint32* ioFrameBuffer)
{
    mIO.SetButtonState(inButtonState);
    mPPU.SetFrameBuffer(ioFrameBuffer);
    
    int cycles = 0;
    
    do
    {
        if (!mIO.IsDMA())
            mCPU.Tick(mPPU.GetClock(), mPPU.GetScanline());
        mPPU.Tick();
        mAPU.Tick();
        mIO.Tick();

        cycles++;
    }
    while (!mPPU.SwapBuffer());
    
    cycles = cycles;
}

//7458.333333333333333