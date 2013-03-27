//
//  APU.cpp
//  nestor
//
//  Created by Tommy de Roos on 3/19/13.
//
//

#include "APU.h"


void BREAK();

const uint8 kLengthIndexTable[] =
{
    0x0A, 0xFE,
    0x14, 0x02,
    0x28, 0x04,
    0x50, 0x06,
    0xA0, 0x08,
    0x3C, 0x0A,
    0x0E, 0x0C,
    0x1A, 0x0E,
    0x0C, 0x10,
    0x18, 0x12,
    0x30, 0x14,
    0x60, 0x16,
    0xC0, 0x18,
    0x48, 0x1A,
    0x10, 0x1C,
    0x20, 0x1E
};


const uint16 kNoisePeriodTable[] =
{
    0x004,
    0x008,
    0x010,
    0x020,
    0x040,
    0x060,
    0x080,
    0x0A0,
    0x0CA,
    0x0FE,
    0x17C,
    0x1FC,
    0x2FA,
    0x3F8,
    0x7F2,
    0xFE4
};

APU::APU() :
    mAPUClock(1),
    mSequencerClock(0),
    mMode(0),
    mInterrupt(false)
{
}


void APU::Tick()
{
    if (--mAPUClock == 0)
        UpdateSequencer();
}


uint16 kSequencerTimers[] =
{
    // 4 step, NTSC
    7456, 7458, 7458, 7458, 0,

    // 5 step NTSC
    7458, 7456, 7458, 7458, 7452
    
    // TODO: PAL
};


void APU::UpdateSequencer()
{
    // Update the sequencer
    bool mode_5_step = (mMode & 0x80) != 0;
    uint8 mod = mode_5_step ? 5 : 4;
    
    if (mSequencerClock != 4)
    {
        ClockEnvelope();
        
        switch (mSequencerClock)
        {
            case 0:
            case 2:
                if (mode_5_step)
                    ClockLength();
                break;
                
            case 1:
            case 3:
                if (!mode_5_step)
                    ClockLength();
                break;
        }
        
        if (mSequencerClock == 3 && (mMode & 0xC0) == 0)
            SetInterrupt();
    }
    
    mAPUClock = kSequencerTimers[mSequencerClock + (mode_5_step?5:0)];
    mSequencerClock = (mSequencerClock + 1) % mod;
}



void APU::ClockEnvelope()
{
    mSquare1.ClockEnvelope();
    mSquare2.ClockEnvelope();
    mTriangle.ClockEnvelope();
    mNoise.ClockEnvelope();
}



void APU::ClockLength()
{
    mSquare1.ClockLength();
    mSquare2.ClockLength();
    mTriangle.ClockLength();
    mNoise.ClockLength();
}




void APU::Load(uint16 inAddr, uint8* outValue) const
{
    if (inAddr != 0x4015)
        BREAK();
    
    *outValue = (mSquare1.mLength  != 0 ? 1<<0 : 0) |
                (mSquare2.mLength  != 0 ? 1<<1 : 0) |
                (mTriangle.mLength != 0 ? 1<<2 : 0) |
                (mNoise.mLength    != 0 ? 1<<3 : 0) |
                (mInterrupt             ? 1<<6 : 0);
    
    mInterrupt = false;
}



void APU::Store(uint16 inAddr, uint8 inValue)
{
    uint8 reg = (uint8)(inAddr - 0x4000);
    
    if (reg < 0x4)
        mSquare1.Store(reg, inValue);
    
    else if (reg < 0x8)
        mSquare2.Store(reg - 0x4, inValue);
    
    else if (reg < 0xC)
        mTriangle.Store(reg - 0x8, inValue);

    else if (reg < 0x10)
        mNoise.Store(reg - 0xC, inValue);

    else if (reg < 0x14)
        mDMC.Store(reg - 0x10, inValue);
    
    else if (reg == 0x15)
    {
        mSquare1.SetLengthCtrEnabled( (inValue & (1 << 0)) != 0);
        mSquare2.SetLengthCtrEnabled( (inValue & (1 << 1)) != 0);
        mTriangle.SetLengthCtrEnabled((inValue & (1 << 2)) != 0);
        mNoise.SetLengthCtrEnabled(   (inValue & (1 << 3)) != 0);
    }

    else if (reg == 0x17)
    {
        mMode = inValue;
        mSequencerClock = 0;
        if (mMode & 0x80)
            mAPUClock = 1;
        else
            mAPUClock = 7459;
        
        // Clear the frame interrupt flag when the interrupt is disabled 
        if ((mMode & 0x40))
            mInterrupt = false;
    }
    
    else
        BREAK();
}



//// Square


void APU::Square::Store(uint8 inReg, uint8 inValue)
{
    switch (inReg)
    {
        case 0:
        case 1:
            mRegisters[inReg] = inValue;
            break;

        case 2:
            mPeriod &= 0x07;
            mPeriod |= inValue << 3;
            break;

        case 3:
            mPeriod &= ~0x07;
            mPeriod |= inValue & 0x07;
            
            if (GetLengthCtrEnabled())
                mLength = kLengthIndexTable[inValue>>3];
            break;
    }
}




void APU::Square::ClockEnvelope()
{
}



void APU::Square::ClockLength()
{
    // Update length if halt not set
    if (GETBIT(mRegisters[0], 5) == 0)
        mLength = mLength == 0 ? 0 : mLength-1;
}







//// Triangle


void APU::Triangle::Store(uint8 inReg, uint8 inValue)
{
    switch (inReg)
    {
        case 0:
            mRegisters[inReg] = inValue;
            break;

        case 1:
            break;
            
        case 2:
            mPeriod &= 0x07;
            mPeriod |= inValue << 3;
            break;
            
        case 3:
            mPeriod &= ~0x07;
            mPeriod |= inValue & 0x07;
            
            if (GetLengthCtrEnabled())
                mLength = kLengthIndexTable[inValue>>3];
            break;
    }
}




void APU::Triangle::ClockEnvelope()
{
}



void APU::Triangle::ClockLength()
{
    // Update length if halt not set
    if (GETBIT(mRegisters[0], 7) == 0)
        mLength = mLength == 0 ? 0 : mLength-1;
}



//// Noise


void APU::Noise::Store(uint8 inReg, uint8 inValue)
{
    switch (inReg)
    {
        case 0:
            mRegisters[inReg] = inValue;
            break;
            
        case 1:
            break;
            
        case 2:
            mShortMode = (inValue & 0x80) != 0;
            mPeriod    = kNoisePeriodTable[inValue & 0x0F];
            break;
            
        case 3:
            if (GetLengthCtrEnabled())
                mLength = kLengthIndexTable[inValue>>3];
            break;
    }
}




void APU::Noise::ClockEnvelope()
{
}



void APU::Noise::ClockLength()
{
    // Update length if halt not set
    if (GETBIT(mRegisters[0], 5) == 0)
        mLength = mLength == 0 ? 0 : mLength-1;
}
