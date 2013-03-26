//
// Created by tderoos on 3/3/13.
//
// To change the template use AppCode | Preferences | File Templates.
//



#ifndef __IO_H_
#define __IO_H_

#include "types.h"

class Ram;
class Rom;
class PPU2C07;
class APU;

class IO {
public:
    
    IO(Ram* inRam, Rom* inRom, PPU2C07* inPPU, APU* inAPU);
    
    void    SetButtonState(char inState) { mButtonState = inState; }

    void    Tick();
    void    Load(uint16_t inAddr, uint8_t* outValue);
    void    Store(uint16_t inAddr, uint8_t inValue);

    void    SetNMI(bool inSet)          { mNMI = inSet; }

    bool    Reset() const               { return mReset; }
    bool    IRQ() const;
    bool    NMI() const                 { return mNMI; }
    
private:
    Ram*        mRam;
    Rom*        mRom;
    PPU2C07*    mPPU;
    APU*        mAPU;

    char        mButtonState;
    UInt8       mButtonReadMask;
    
    enum EDMAState{
        READ, WRITE, INACTIVE
    };
    EDMAState   mDMAState;
    uint16_t    mDMASrc;
    UInt8       mDMAValue;
    
    UInt8       mPRGRam[0x2000];
    
    bool        mReset;
    bool        mNMI;
    bool        mIRQ;
};

#endif //__IO_H_
