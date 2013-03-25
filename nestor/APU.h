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
    void    Update();
    
    // Access
    void    Load(UInt16 inAddr, UInt8* outValue) const;
    void    Store(UInt16 inAddr, UInt8 inValue);
    
    // Interrupt
    bool    GetInterrupt()                                  { return mInterrupt; }
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
        
        void    Store(UInt8 inAddr, UInt8 inValue);
        
        void    ClockEnvelope();
        void    ClockLength();
        
        UInt16  mPeriod;
        bool    mLengthEnabled;
        UInt8   mLength;

        UInt8   mRegisters[2];
    };
    Square  mSquare1;
    Square  mSquare2;
    
    struct Triangle
    {
        inline void SetLengthCtrEnabled(bool inValue)       { mLengthEnabled = inValue; if (!inValue) mLength = 0; }
        inline bool GetLengthCtrEnabled() const             { return mLengthEnabled; }
        
        void    Store(UInt8 inAddr, UInt8 inValue);

        void    ClockEnvelope();
        void    ClockLength();

        UInt16  mPeriod;
        bool    mLengthEnabled;
        UInt8   mLength;
        
        UInt8   mRegisters[1];
    };
    Triangle mTriangle;
    
    struct Noise
    {
        inline void SetLengthCtrEnabled(bool inValue)       { mLengthEnabled = inValue; if (!inValue) mLength = 0; }
        inline bool GetLengthCtrEnabled() const             { return mLengthEnabled; }
        
        void    Store(UInt8 inAddr, UInt8 inValue);
        
        void    ClockEnvelope();
        void    ClockLength();

        UInt16  mPeriod;
        bool    mLengthEnabled;
        UInt8   mLength;
        bool    mShortMode;

        UInt8   mRegisters[1];
    };
    Noise mNoise;
    
    struct DMC
    {
        void    Store(UInt8 inAddr, UInt8 inValue)          { }
        
    };
    DMC mDMC;

    enum EAPURegister {
        NUM_APU_REGISTERS = 0x18
    };

    UInt8   mRegisters[NUM_APU_REGISTERS];
    
    UInt8   mMode;
    UInt8   mSequencerClock;
    mutable bool    mInterrupt;
};


#endif /* defined(__nestor__APU__) */
