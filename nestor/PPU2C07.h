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
            PPU2C07(Rom* inRom);

    void    Scanline(UInt32* ioFrameBuffer);
    
    bool    GetNMI() const                                      { return (mPPUCtrl & 0x80) != 0 && mScanline == 241; }
    UInt16  GetScanline() const                                 { return mScanline; }

    // Access
    void    Load(UInt16 inAddr, UInt8* outValue) const;
    void    Store(UInt16 inAddr, UInt8 inValue);

private:
    
    // Scanline sprite span
    struct ScanlineSprite
    {
        UInt8 mX;
        UInt8 mPlane0;
        UInt8 mPlane1;
        UInt8 mPriority;
        UInt8 mPalette;
        UInt8 mForeGround;
    };

    void        UpdateMirroring();
    int         FetchScanlineSprites(ScanlineSprite* ioSprites);
    
    const Rom*  mRom;
    
    UInt8       mVRAM[0x4000];
    UInt8       mOAM[64*4];
    UInt8*      mCHRData;
    UInt16      mScanline;
    
    UInt16      mNameTable[4];
    
    // Internal registers
    mutable bool mPPUAddrWriteLO;
    
    UInt8 mPPUCtrl;
    UInt8 mPPUMask;
    
    mutable UInt8  mPPULoadBuffer;
    mutable UInt8  mPPUStatus;
    mutable UInt16 mPPUAddr;
    mutable UInt16 mPPUScroll;
    mutable UInt8  mOAMAddr;
};

#endif //__PPU2C07_H_
