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
    void Tick(uint16 inPPUClock, int16 inScanline);

//private:

    uint8   Load(uint16 inAddr);
    void    Store(uint16 inAddr, uint8 inValue);

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
        uint16 mPC;

        union
        {
            struct {
                uint8  mSP;
                uint8  mA;
                uint8  mX;
                uint8  mY;
                uint8  mRegZero;
            };
            uint8 mReg[5];
        };

        union
        {
            struct
            {
                uint8 mCarry      : 1;
                uint8 mZero       : 1;
                uint8 mInterrupt  : 1;
                uint8 mDecimal    : 1;
                uint8 mBreak      : 1;
                uint8 mReserved   : 1;
                uint8 mOverflow   : 1;
                uint8 mNeg        : 1;
            };
            uint8 mFlags;
        };
    };

    uint8       Handle00(uint8 opcode);
    uint8       Handle01(uint8 opcode);
    uint8       Handle10(uint8 opcode);
    
    bool        mNMI;

    uint8       mInstrTimer;
    Status      mRegs;
    IO*         mIO;
};

#endif //__CPU6502_H_
