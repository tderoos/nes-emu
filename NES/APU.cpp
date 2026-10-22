// nes-emu APU Module
#include "APU.h"
#include <math.h>
#include <assert.h>


static int clocks = 0;

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
    mSquare1(0),
    mSquare2(0),
    mAudioBuffer(nullptr),
    mAudioBufferOffset(0),
    mDACDivider(2),
    mAPUClock(1),
    mSequencerClock(0),
    mMode(0),
    mInterrupt(false)
{
    mNoise.Reset();
}



uint8 sSquareDuty[4][8] =
{
    { 0, 1, 0, 0, 0, 0, 0, 0 },
    { 0, 1, 1, 0, 0, 0, 0, 0 },
    { 0, 1, 1, 1, 1, 0, 0, 0 },
    { 1, 0, 0, 1, 1, 1, 1, 1 }
};

uint8 sTriangleDuty[] =
{
    15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15
};

static int count;

void APU::SetAudioBuffer(uint8* ioAudioBuffer)
{
    mAudioBuffer = ioAudioBuffer;
    mAudioBufferOffset = 0;
}



void APU::Tick()
{
    mNoise.Tick();
    
    clocks++;
    if (--mAPUClock == 0)
        UpdateSequencer();
    
    mDACDivider -= 2;
    if (mDACDivider <= 1)
        UpdateDAC();
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
    mSquare1.mEnvelope.Tick();
    mSquare2.mEnvelope.Tick();
    mNoise.mEnvelope.Tick();
}



void APU::ClockLength()
{
    mSquare1.ClockLength();
    mSquare2.ClockLength();
    mTriangle.ClockLength();
    mNoise.ClockLength();
}



void APU::UpdateDAC()
{
    assert(mAudioBufferOffset < 736);

    mDACDivider += 81;
    count++;
    
    float sq1   = mSquare1.ClockDAC();
    float sq2   = mSquare2.ClockDAC();
    float tr    = mTriangle.ClockDAC() / 8227.0f;
    float noise = mNoise.ClockDAC()    / 12241.0f;
    float dmc   = mDMC.ClockDAC()      / 22638.0f;
    
    float sq_out = 95.88f / ((8128.0f / (sq1+sq2)) + 100.0f);
    float tnd_out = 159.79f / ((1.0f / (tr+noise+dmc)) + 100.0f);
    
    uint8 v8 = (uint8)((sq_out + tnd_out) * 127.0f);

    if (mAudioBuffer != nullptr)
        mAudioBuffer[mAudioBufferOffset++] = v8;
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

        // I have to add 1 to the timers - I suspect it's an
        // update order issue. To investigate. 
        if (mMode & 0x80)
            mAPUClock = 1+1;
        else
            mAPUClock = 7459+1;
        
        clocks = 0;
        
        // Clear the frame interrupt flag when the interrupt is disabled 
        if ((mMode & 0x40))
            mInterrupt = false;
    }
    
    else
        BREAK();
}



// State saving
void APU::ReadState(const SaveState& ioState)
{
}
void APU::WriteState(SaveState& ioState) const
{
}



/// EnvelopeGenerator

void APU::EnvelopeGenerator::Tick()
{
    if (mReset)
    {
        mDivider = (mRegister & 0x0F) + 1;
        mValue   = 15;
        mReset   = false;
    }
    else
    {
        mDivider--;
        if (mDivider == 0)
        {
            mDivider = (mRegister & 0x0F) + 1;
            
            if (mValue == 0)
            {
                // Check for looping
                if (GETBIT(mRegister, 5))
                    mValue = 15;
            }
            else
                mValue--;
        }
    }
}

// SweepUnit
void APU::SweepUnit::Tick(int inIdx, uint16& ioPeriod)
{
    mDivider--;
    if (GETBIT(mRegister, 7) && mDivider == 0)
    {
        mDivider = ((mRegister >> 4) & 0x07) + 1;
        
        uint8 shift = mRegister&0x07;
        uint16 v = ioPeriod >> shift;
        
        if (GETBIT(mRegister, 3))
            v = ~v + inIdx;
        
        ioPeriod = (ioPeriod + v) & 0x3FFF;
    }
    
    if (mReset)
    {
        mDivider = ((mRegister >> 4) & 0x07) + 1;
        mReset   = false;
    }
}


//// Square


void APU::Square::Store(uint8 inReg, uint8 inValue)
{
    switch (inReg)
    {
        case 0:
            mEnvelope.Set(inValue);
            mRegisters[inReg] = inValue;
            break;
            
        case 1:
            mSweep.Set(inValue);
            mRegisters[inReg] = inValue;
            break;

        case 2:
            mPeriod &= 0xFF00;
            mPeriod |= inValue;
            break;

        case 3:
            mPeriod &= 0x00FF;
            mPeriod |= (inValue & 0x07) << 8;

            
            if (GetLengthCtrEnabled())
                mLength = kLengthIndexTable[inValue>>3];
            break;
    }
}





void APU::Square::ClockLength()
{
    // Update length if halt not set
    if (GETBIT(mRegisters[0], 5) == 0)
        mLength = mLength == 0 ? 0 : mLength-1;
    
    mSweep.Tick(mIdx, mPeriod);
}



float APU::Square::ClockDAC()
{
    double fr = 1789773.0;
    fr /= (16.0 * (mPeriod+1));
    
    mPhase = (float)fmod(mPhase + (fr / 44100.0f), 1.0f);

    int idx = (int)(mPhase * 8);
    float v = sSquareDuty[mRegisters[0] >> 6][idx] == 1 ? 15.0f : 0.0f;
    
    v = mLength != 0 ? v : 0.0f;
    v *= mEnvelope.Volume();
    
    return v;
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
            mPeriod &= 0xFF00;
            mPeriod |= inValue;
            break;
            
        case 3:
            mPeriod &= 0x00FF;
            mPeriod |= (inValue & 0x07) << 8;
            
            if (GetLengthCtrEnabled())
                mLength = kLengthIndexTable[inValue>>3];
            break;
    }
}



void APU::Triangle::ClockLength()
{
    // Update length if halt not set
    if (GETBIT(mRegisters[0], 7) == 0)
        mLength = mLength == 0 ? 0 : mLength-1;
}


float APU::Triangle::ClockDAC()
{
    double fr = 1789773.0;
    fr /= (32.0 * (mPeriod+1));
    
    mPhase = (float)fmod(mPhase + (fr / 44100.0f), 1.0f);
    
    int idx = (int)(mPhase * 32);
    float v = sTriangleDuty[idx];
    v = mLength != 0 ? v : 0.0f;
    
    return v;
}




//// Noise


void APU::Noise::Reset()
{
    mDivider = 0;
    mNoise = 1;
}


void APU::Noise::Tick()
{
    if (mDivider == 0)
    {
        bool mode = mRegisters[2] & 0x80;
        uint16 v = mNoise ^ (mNoise >> (mode == 0 ? 1 : 6));
        mNoise = (mNoise >> 1) | (v << 14);
        mDivider = mPeriod;
    }

    mDivider--;
}


void APU::Noise::Store(uint8 inReg, uint8 inValue)
{
    switch (inReg)
    {
        case 0:
            mEnvelope.Set(inValue);
            mRegisters[inReg] = inValue;
            break;
            
        case 1:
            break;
            
        case 2:
            mShortMode = (inValue & 0x80) != 0;
            mPeriod    = kNoisePeriodTable[inValue & 0x0F];
            mDivider   = 0;
            break;
            
        case 3:
            if (GetLengthCtrEnabled())
                mLength = kLengthIndexTable[inValue>>3];
            break;
    }
}



void APU::Noise::ClockLength()
{
    // Update length if halt not set
    if (GETBIT(mRegisters[0], 5) == 0)
        mLength = mLength == 0 ? 0 : mLength-1;
}


float APU::Noise::ClockDAC()
{
    if (mLength != 0)
        return mEnvelope.Volume() *  (mNoise & 0x01) ? 15.0f : 0.0f;
    return 0.0f;
}




float APU::DMC::ClockDAC()
{
    return 0.0f;
}

