//
//  APU.h
//  nestor
//
//  Created by Tommy de Roos on 3/19/13.
//
//

#ifndef __nestor__APU__
#define __nestor__APU__

#include "types.h"

#define SETBIT(reg, bit, value)     { reg &= ~(1<<(bit)); reg |= (value)<<(bit); }
#define GETBIT(reg, bit)            ((reg) >> (bit) & 1)

class APU
{
public:
            APU();
    void    SetAudioBuffer(uint8* ioAudioBuffer);
    uint32  GetSampleCount() const                          { return mAudioBufferOffset; }

    void    Tick();
    
    // Access
    void    Load(uint16 inAddr, uint8* outValue) const;
    void    Store(uint16 inAddr, uint8 inValue);
    
    // Interrupt
    bool    GetInterrupt() const                            { return mInterrupt; }
    void    SetInterrupt()                                  { mInterrupt = true; }
    void    ClearInterrupt()                                { mInterrupt = false; }
    
private:
    
    void    UpdateSequencer();
    void    ClockEnvelope();
    void    ClockLength();
    void    UpdateDAC();
    
    struct Channel
    {
        uint8   mRegister[4];
        
        uint16  Period()                                    { return mRegister[2] | (mRegister[3]&0x07)<<8; }
    };
    
    struct EnvelopeGenerator
    {
        void    Tick();
        void    Set(uint8 inValue)                          { mRegister = inValue; mReset = true; }
        float   Volume() const                              { return (float)(GETBIT(mRegister, 4) ? mRegister & 0x0F : mValue) / 15.0f; }
        
        uint8   mRegister;
        uint8   mDivider;
        uint8   mValue;
        bool    mReset;
    };
    
    struct SweepUnit
    {
        void    Tick(int inIdx, uint16& ioPeriod);
        void    Set(uint8 inValue)                          { mRegister = inValue; mReset = true; }

        uint8   mRegister;
        uint8   mDivider;
        bool    mReset;
    };
    
    
    struct Square
    {
        Square(uint8 inIdx) : mIdx(inIdx), mPhase(0.0f)     { }
        
        inline void SetLengthCtrEnabled(bool inValue)       { mLengthEnabled = inValue; if (!inValue) mLength = 0; }
        inline bool GetLengthCtrEnabled() const             { return mLengthEnabled; }
        
        void    Store(uint8 inAddr, uint8 inValue);
        
        void    ClockLength();
        float   ClockDAC();
        
        EnvelopeGenerator mEnvelope;
        SweepUnit         mSweep;
        
        uint8   mIdx;
        uint16  mPeriod;
        bool    mLengthEnabled;
        uint8   mLength;

        uint8   mRegisters[2];
        
        float   mPhase;
    };
    Square  mSquare1;
    Square  mSquare2;
    
    struct Triangle
    {   
        Triangle() : mPhase(0.0f)                           { }
        
        inline void SetLengthCtrEnabled(bool inValue)       { mLengthEnabled = inValue; if (!inValue) mLength = 0; }
        inline bool GetLengthCtrEnabled() const             { return mLengthEnabled; }
        
        void    Store(uint8 inAddr, uint8 inValue);

        void    ClockLength();
        float   ClockDAC();

        uint16  mPeriod;
        bool    mLengthEnabled;
        uint8   mLength;
        
        uint8   mRegisters[1];
        float   mPhase;
    };
    Triangle mTriangle;
    
    struct Noise
    {
        void    Reset();
        void    Tick();
        
        inline void SetLengthCtrEnabled(bool inValue)       { mLengthEnabled = inValue; if (!inValue) mLength = 0; }
        inline bool GetLengthCtrEnabled() const             { return mLengthEnabled; }
        
        void    Store(uint8 inAddr, uint8 inValue);
        
        void    ClockLength();
        float   ClockDAC();
        
        EnvelopeGenerator mEnvelope;

        uint16  mPeriod;
        bool    mLengthEnabled;
        uint8   mLength;
        bool    mShortMode;
        uint16  mNoise;
        uint16  mDivider;
        

        uint8   mRegisters[1];
    };
    Noise mNoise;
    
    struct DMC
    {
        void    Store(uint8 inAddr, uint8 inValue)          { }
        float   ClockDAC();
    };
    DMC mDMC;

    enum EAPURegister {
        NUM_APU_REGISTERS = 0x18
    };

    uint8*  mAudioBuffer;
    uint32  mAudioBufferOffset;
    uint32  mDACDivider;
    
    uint8   mRegisters[NUM_APU_REGISTERS];
    
    uint16  mAPUClock;
    uint8   mMode;
    uint8   mSequencerClock;
    mutable bool    mInterrupt;
};


#endif /* defined(__nestor__APU__) */
