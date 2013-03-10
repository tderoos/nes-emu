//
// Created by tderoos on 3/3/13.
//
// To change the template use AppCode | Preferences | File Templates.
//


#include "IO.h"
#include "Rom.h"
#include "Ram.h"
#include "PPU2C07.h"


IO::IO(Ram* inRam, Rom* inRom, PPU2C07* inPPU) :
    mRam(inRam),
    mRom(inRom),
    mPPU(inPPU),

    mButtonState(0),
    mButtonReadMask(0),

    mDMAState(INACTIVE),
    mDMASrc(0x0000),
    mDMAValue(0),

    mReset(true)
{
}


void IO::Tick()
{
    mReset = false;
    mNMI   = false;
    
    switch (mDMAState)
    {
        case READ:
            Load(mDMASrc++, &mDMAValue);
            mDMAState = WRITE;
            break;
            
        case WRITE:
            mPPU->Store(0x2004, mDMAValue);
            mDMAState = (mDMASrc & 0xFF) ? READ : INACTIVE;
            break;
            
        case INACTIVE:
            break;
    }
}



void IO::Load(uint16_t inAddr, uint8_t* outValue)
{
    if (inAddr < 0x2000)
        mRam->Load(inAddr, outValue);

    else if (inAddr < 0x4000)
        mPPU->Load(inAddr, outValue);
    
    // Controller
    else if (inAddr == 0x4016)
    {
        // Set b5 (connected)
        *outValue = 0x40;
        if (mButtonState & mButtonReadMask)
            *outValue += 1;
        mButtonReadMask <<= 1;
    }
    else if (inAddr == 0x4017)
        *outValue = 0x40;
    
    
    else if (inAddr < 0x4020)
    {   // APU
    }
    else if (inAddr < 0x6000)
    {   // EROM
    }
    else if (inAddr < 0x8000)
    {   // SRAM
    }
    else
        mRom->Load(inAddr, outValue);
    
    static bool print = false;
    
    if (print && inAddr >= 0x2000 && inAddr < 0x2003)
        printf("load 0x%x -> $%x\n", inAddr, *outValue);
}

bool IO::pushing = false;

void IO::Store(uint16_t inAddr, uint8_t inValue)
{
    if (inAddr < 0x0200 && inAddr >= 0x1F0 && !pushing)
        inAddr = inAddr;
    
    if (inAddr < 0x2000)
        mRam->Store(inAddr, inValue);

    else if (inAddr < 0x4000)
        mPPU->Store(inAddr, inValue);

    // DMA
    else if (inAddr == 0x4014)
    {
        mDMASrc   = inValue << 8;
        mDMAState = READ;
    }

    
    // Controller strobe
    else if (inAddr == 0x4016)
    {
        if ((inValue&1) == 0)
            mButtonReadMask = 1;
    }
    
    else if (inAddr < 0x4020)
    {   // APU
    }
    else if (inAddr < 0x6000)
    {   // EROM
    }
    else if (inAddr < 0x8000)
    {   // SRAM
    }
    else
        mRom->Store(inAddr, inValue);
}
