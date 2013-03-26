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
    void    SetFrameBuffer(UInt32* inFrameBuffer)               { mFrameBuffer = inFrameBuffer; }

    void    Tick();
    void    Scanline();
    
    bool    GetNMI() const                                      { return !((mPPUCtrl & 0x80) != 0 && (mPPUStatus & 0x80) != 0); }
    UInt16  GetScanline() const                                 { return mScanline; }
    UInt16  GetClock() const                                    { return mClock; }
    bool    SwapBuffer() const                                  { return mScanline == 241 && mClock < 3; }

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
    UInt32*     mFrameBuffer;
    UInt16      mClock;

    
    UInt8       mVRAM[0x4000];
    UInt8       mOAM[64*4];
    UInt8*      mCHRData;
    UInt16      mScanline;
    
    UInt16      mNameTable[4];
    
    // Internal registers
    mutable bool mPPUAddrWriteLO;
    
    UInt8 mPPUCtrl;
    UInt8 mPPUMask;
    
    // Loopy scrolling
    // http://wiki.nesdev.com/w/index.php/The_skinny_on_NES_scrolling
    mutable UInt16 mV;              // Current VRam (15b)
    mutable UInt16 mT;              // Temp VRam (15b)
    mutable UInt8  mX;              // Fine X (3b)
    mutable UInt8  mW;              // Write toggle (1b)
    
    enum
    {
        ENameTableShiftSrc  = 0,
        ENameTableMaskSrc   = (0x3 << ENameTableShiftSrc),
        ENameTableShiftTgt  = 10,
        ENameTableMaskTgt   = (0x3 << ENameTableShiftTgt),
        
        EScrollXFineMaskSrc    = 0x7,
        EScrollXCoarseShiftSrc = 3,
        EScrollXCoarseMaskSrc  = 0x1F << EScrollXCoarseShiftSrc,
        EScrollXCoarseShiftTgt = 0,
        EScrollXCoarseMaskTgt  = 0x1F << EScrollXCoarseShiftTgt,
        
        EScrollYFineMaskSrc    = 0x7,
        EScrollYFineShiftTgt   = 12,
        EScrollYFineMaskTgt    = EScrollYFineMaskSrc << EScrollYFineShiftTgt,
        
        EScrollYCoarseShiftSrc = 3,
        EScrollYCoarseMaskSrc  = 0x1F << EScrollYCoarseShiftSrc,
        EScrollYCoarseShiftTgt = 5,
        EScrollYCoarseMaskTgt  = 0x1F << EScrollYCoarseShiftTgt,

        EAddrLoShiftSrc        = 0,
        EAddrLoMaskSrc         = 0x3F << EAddrLoShiftSrc,
        EAddrLoShiftTgt        = 8,
        EAddrLoMaskTgt         = 0xFF << EAddrLoShiftTgt,

        EAddrHiShiftSrc        = 0,
        EAddrHiMaskSrc         = 0xFF << EAddrLoShiftSrc,
        EAddrHiShiftTgt        = 0,
        EAddrHiMaskTgt         = 0xFF << EAddrLoShiftTgt
    };
    
    mutable UInt8  mPPULoadBuffer;
    mutable UInt8  mPPUStatus;
    mutable UInt16 mPPUAddr;
    mutable UInt16 mPPUScroll;
    mutable UInt8  mOAMAddr;
};

#endif //__PPU2C07_H_
