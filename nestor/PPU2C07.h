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
    void    SetFrameBuffer(uint32* inFrameBuffer)               { mFrameBuffer = inFrameBuffer; }

    void    Tick();
    void    Scanline();
    
    bool    GetNMI() const                                      { return !((mPPUCtrl & 0x80) != 0 && (mPPUStatus & 0x80) != 0); }
    int16   GetScanline() const                                 { return mScanline; }
    uint16  GetClock() const                                    { return mClock; }
    bool    SwapBuffer() const                                  { return mScanline == 241 && mClock < 3; }
    
    inline bool IsRendering() const                             { return (mScanline >= 0 && mScanline < 240 && (mPPUMask & 0x18) != 0); }


    // Access
    void    Load(uint16 inAddr, uint8* outValue) const;
    void    Store(uint16 inAddr, uint8 inValue);

private:
    
    // Scanline sprite span
    struct ScanlineSprite
    {
        uint8 mX;
        uint8 mPlane0;
        uint8 mPlane1;
        uint8 mPriority;
        uint8 mPalette;
        uint8 mForeGround;
    };

    void        UpdateMirroring();
    int         FetchScanlineSprites(ScanlineSprite* ioSprites);
    uint16      IncreaseScrollY(uint16 inV) const;
    
    const Rom*  mRom;
    uint32*     mFrameBuffer;
    uint16      mClock;

    
    uint8       mVRAM[0x4000];
    uint8       mOAM[64*4];
    uint8*      mCHRData;
    int16       mScanline;
    
    uint16      mNameTable[4];
    
    uint8 mPPUCtrl;
    uint8 mPPUMask;
    
    // Loopy scrolling
    // http://wiki.nesdev.com/w/index.php/The_skinny_on_NES_scrolling
    mutable uint16 mV;              // Current VRam (15b)
    mutable uint16 mT;              // Temp VRam (15b)
    mutable uint8  mX;              // Fine X (3b)
    mutable uint8  mW;              // Write toggle (1b)
    
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
        EAddrHiMaskSrc         = 0xFF << EAddrHiShiftSrc,
        EAddrHiShiftTgt        = 0,
        EAddrHiMaskTgt         = 0xFF << EAddrHiShiftTgt
    };
    
    mutable uint8  mPPULoadBuffer;
    mutable uint8  mPPUStatus;
    mutable uint16 mPPUScroll;
    mutable uint8  mOAMAddr;
};

#endif //__PPU2C07_H_
