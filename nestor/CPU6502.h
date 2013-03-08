//
// Created by tderoos on 3/3/13.
//
// To change the template use AppCode | Preferences | File Templates.
//



#ifndef __CPU6502_H_
#define __CPU6502_H_

#include <iostream>

class IO;


class CPU6502 {

public:
    CPU6502(IO* inIO);
    void Tick();

//private:

    uint8_t Load(uint16_t inAddr);
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
        uint16_t mPC;

        union
        {
            struct {
                uint8_t  mSP;
                uint8_t  mAcc;
                uint8_t  mX;
                uint8_t  mY;
                uint8_t  mRegZero;
            };
            uint8_t mReg[5];
        };

        union
        {
            struct
            {
                uint8_t mCarry      : 1;
                uint8_t mZero       : 1;
                uint8_t mInterrupt  : 1;
                uint8_t mDecimal    : 1;
                uint8_t mBreak      : 1;
                uint8_t mReserved   : 1;
                uint8_t mOverflow   : 1;
                uint8_t mNeg        : 1;
            };
            uint8_t mFlags;
        };
    };

    typedef   void (*OpcodeHandler)(uint8_t inHi, Status& ioStatus, IO* ioIO);
    OpcodeHandler mHandlers[16];
    
    void        Handle00(uint8_t opcode);
    void        Handle01(uint8_t opcode);
    void        Handle10(uint8_t opcode);

    bool        mOldMode;
    Status      mRegs;
    IO*         mIO;
    
    // Debugging
    struct DebugTrace
    {
        uint16_t mAddr;
        uint8_t  mOp;
    };
    
    void BREAK() const;
    DebugTrace  mTrace[256];
    int         mTraceHead;
};

#endif //__CPU6502_H_
