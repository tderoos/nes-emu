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
    
    struct Square
    {
        inline void SetLengthCtrEnabled(bool inValue)       { mLengthEnabled = inValue; if (!inValue) mLength = 0; }
        inline bool GetLengthCtrEnabled() const             { return mLengthEnabled; }
        
        void    Store(uint8 inAddr, uint8 inValue);
        
        void    ClockEnvelope();
        void    ClockLength();
        
        uint16  mPeriod;
        bool    mLengthEnabled;
        uint8   mLength;

        uint8   mRegisters[2];
    };
    Square  mSquare1;
    Square  mSquare2;
    
    struct Triangle
    {
        inline void SetLengthCtrEnabled(bool inValue)       { mLengthEnabled = inValue; if (!inValue) mLength = 0; }
        inline bool GetLengthCtrEnabled() const             { return mLengthEnabled; }
        
        void    Store(uint8 inAddr, uint8 inValue);

        void    ClockEnvelope();
        void    ClockLength();

        uint16  mPeriod;
        bool    mLengthEnabled;
        uint8   mLength;
        
        uint8   mRegisters[1];
    };
    Triangle mTriangle;
    
    struct Noise
    {
        inline void SetLengthCtrEnabled(bool inValue)       { mLengthEnabled = inValue; if (!inValue) mLength = 0; }
        inline bool GetLengthCtrEnabled() const             { return mLengthEnabled; }
        
        void    Store(uint8 inAddr, uint8 inValue);
        
        void    ClockEnvelope();
        void    ClockLength();

        uint16  mPeriod;
        bool    mLengthEnabled;
        uint8   mLength;
        bool    mShortMode;

        uint8   mRegisters[1];
    };
    Noise mNoise;
    
    struct DMC
    {
        void    Store(uint8 inAddr, uint8 inValue)          { }
        
    };
    DMC mDMC;

    enum EAPURegister {
        NUM_APU_REGISTERS = 0x18
    };

    uint8*  mAudioBuffer;
    uint32  mAudioBufferOffset;
    
    uint8   mRegisters[NUM_APU_REGISTERS];
    
    uint16  mAPUClock;
    uint8   mMode;
    uint8   mSequencerClock;
    mutable bool    mInterrupt;
};


#endif /* defined(__nestor__APU__) */
