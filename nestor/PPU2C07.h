//
// Created by tderoos on 3/3/13.
//
// To change the template use AppCode | Preferences | File Templates.
//



#ifndef __PPU2C07_H_
#define __PPU2C07_H_

#include <iostream>
#include "Rom.h"


class PPU2C07 {
public:
    PPU2C07(const Rom* inRom);

    void Tick();
    void Scanline(uint32_t* ioFrameBuffer);
    
    bool GetNMI() const                                     { return (mPPUStatus & 0x80) != 0 && mScanline == 241; }
    uint16_t GetScanline() const                            { return mScanline; }

    // Access
    void Load(uint16_t inAddr, uint8_t* outValue) const;
    void Store(uint16_t inAddr, uint8_t inValue);

private:
    const Rom* mRom;
    uint8_t mVRAM[0x4000];
    uint8_t mOAM[64*4];

    uint16_t mScanline;
    
    // Internal registers
    mutable bool mPPUAddrWriteLO;
    
    uint8_t mPPUCtrl;
    uint8_t mPPUMask;
    mutable uint8_t  mPPULoadBuffer;
    mutable uint8_t  mPPUStatus;
    mutable uint16_t mPPUAddr;
    mutable uint16_t mPPUScroll;
    mutable uint8_t  mOAMAddr;
};

#endif //__PPU2C07_H_
