//
// Created by tderoos on 3/3/13.
//
// To change the template use AppCode | Preferences | File Templates.
//



#ifndef __CPU6502_H_
#define __CPU6502_H_

#include <iostream>
#include "types.h"

class IO;

void BREAK();


class CPU6502 {

public:
    CPU6502(IO* inIO);
    void Tick();

//private:

    UInt8   Load(uint16_t inAddr);
    void    Store(uint16_t inAddr, uint8_t inValue);

    enum ERegister
    {
        SP = 0,
        A,
        X,
        Y,
        ZERO
    };

    struct Status
    {
        UInt16 mPC;

        union
        {
            struct {
                UInt8  mSP;
                UInt8  mAcc;
                UInt8  mX;
                UInt8  mY;
                UInt8  mRegZero;
            };
            UInt8 mReg[5];
        };

        union
        {
            struct
            {
                UInt8 mCarry      : 1;
                UInt8 mZero       : 1;
                UInt8 mInterrupt  : 1;
                UInt8 mDecimal    : 1;
                UInt8 mBreak      : 1;
                UInt8 mReserved   : 1;
                UInt8 mOverflow   : 1;
                UInt8 mNeg        : 1;
            };
            UInt8 mFlags;
        };
    };

    UInt8       Handle00(uint8_t opcode);
    UInt8       Handle01(uint8_t opcode);
    UInt8       Handle10(uint8_t opcode);

    UInt8       mInstrTimer;
    Status      mRegs;
    IO*         mIO;
};

#endif //__CPU6502_H_
