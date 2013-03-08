//
// Created by tderoos on 3/3/13.
//
// To change the template use AppCode | Preferences | File Templates.
//



#ifndef __IO_H_
#define __IO_H_

#include <iostream>

class Ram;
class Rom;
class PPU2C07;

class IO {
public:
    
    IO(Ram* inRam, Rom* inRom, PPU2C07* inPPU);

    void Tick();
    void Load(uint16_t inAddr, uint8_t* outValue);
    void Store(uint16_t inAddr, uint8_t inValue);

    void    SetNMI(bool inSet)   { mNMI = inSet; }
    
    bool    Reset() const        { return mReset; }
    bool    NMI() const          { return mNMI; }
    
    static bool pushing;

private:
    Ram* mRam;
    Rom* mRom;
    PPU2C07* mPPU;
    
    bool mReset;
    bool mNMI;
};

#endif //__IO_H_
