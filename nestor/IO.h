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
    void    Load(uint16 inAddr, uint8* outValue);
    void    Store(uint16 inAddr, uint8 inValue);

    bool    IsDMA() const                { return mDMAState != INACTIVE; }
    bool    IRQ() const;
    bool    NMI() const;
    
private:
    Ram*        mRam;
    Rom*        mRom;
    PPU2C07*    mPPU;
    APU*        mAPU;

    char        mButtonState;
    uint8       mButtonReadMask;
    
    enum EDMAState{
        READ, WRITE, INACTIVE
    };
    EDMAState   mDMAState;
    uint16      mDMASrc;
    uint8       mDMAValue;
    
    uint8       mPRGRam[0x2000];
};

#endif //__IO_H_
