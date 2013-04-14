//
// Created by tderoos on 3/3/13.
//
// To change the template use AppCode | Preferences | File Templates.
//

#include "types.h"

/*
const uint32 palette[64] = {
    0x808080, 0x0000BB, 0x3700BF, 0x8400A6,
    0xBB006A, 0xB7001E, 0xB30000, 0x912600,
    0x7B2B00, 0x003E00, 0x00480D, 0x003C22,
    0x002F66, 0x000000, 0x050505, 0x050505,
    
    0xC8C8C8, 0x0059FF, 0x443CFF, 0xB733CC,
    0xFF33AA, 0xFF375E, 0xFF371A, 0xD54B00,
    0xC46200, 0x3C7B00, 0x1E8415, 0x009566,
    0x0084C4, 0x111111, 0x090909, 0x090909,
    
    0xFFFFFF, 0x0095FF, 0x6F84FF, 0xD56FFF,
    0xFF77CC, 0xFF6F99, 0xFF7B59, 0xFF915F,
    0xFFA233, 0xA6BF00, 0x51D96A, 0x4DD5AE,
    0x00D9FF, 0x666666, 0x0D0D0D, 0x0D0D0D,
    
    0xFFFFFF, 0x84BFFF, 0xBBBBFF, 0xD0BBFF,
    0xFFBFEA, 0xFFBFCC, 0xFFC4B7, 0xFFCCAE,
    0xFFD9A2, 0xCCE199, 0xAEEEB7, 0xAAF7EE,
    0xB3EEFF, 0xDDDDDD, 0x111111, 0x111111
};

 /*/
unsigned int sNesPalette[] =
{
    0x7C7C7C, 0x0000FC, 0x0000BC, 0x4428BC,
    0x940084, 0xA80020, 0xA81000, 0x881400,
    0x503000, 0x007800, 0x006800, 0x005800,
    0x004058, 0x000000, 0x000000, 0x000000,

    0xBCBCBC, 0x0078F8, 0x0058F8, 0x6844FC,
    0xD800CC, 0xE40058, 0xF83800, 0xE45C10,
    0xAC7C00, 0x00B800, 0x00A800, 0x00A844,
    0x008888, 0x000000, 0x000000, 0x000000,
   
    0xF8F8F8, 0x3CBCFC, 0x6888FC, 0x9878F8,
    0xF878F8, 0xF85898, 0xF87858, 0xFCA044,
    0xF8B800, 0xB8F818, 0x58D854, 0x58F898,
    0x00E8D8, 0x787878, 0x000000, 0x000000,

    0xFCFCFC, 0xA4E4FC, 0xB8B8F8, 0xD8B8F8,
    0xF8B8F8, 0xF8A4C0, 0xF0D0B0, 0xFCE0A8,
    0xF8D878, 0xD8F878, 0xB8F8B8, 0xB8F8D8,
    0x00FCFC, 0xF8D8F8, 0x000000, 0x000000
};
//*/

static const uint8 BitReverseTable256[] =
{
    0x00, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0, 0x10, 0x90, 0x50, 0xD0, 0x30, 0xB0, 0x70, 0xF0,
    0x08, 0x88, 0x48, 0xC8, 0x28, 0xA8, 0x68, 0xE8, 0x18, 0x98, 0x58, 0xD8, 0x38, 0xB8, 0x78, 0xF8,
    0x04, 0x84, 0x44, 0xC4, 0x24, 0xA4, 0x64, 0xE4, 0x14, 0x94, 0x54, 0xD4, 0x34, 0xB4, 0x74, 0xF4,
    0x0C, 0x8C, 0x4C, 0xCC, 0x2C, 0xAC, 0x6C, 0xEC, 0x1C, 0x9C, 0x5C, 0xDC, 0x3C, 0xBC, 0x7C, 0xFC,
    0x02, 0x82, 0x42, 0xC2, 0x22, 0xA2, 0x62, 0xE2, 0x12, 0x92, 0x52, 0xD2, 0x32, 0xB2, 0x72, 0xF2,
    0x0A, 0x8A, 0x4A, 0xCA, 0x2A, 0xAA, 0x6A, 0xEA, 0x1A, 0x9A, 0x5A, 0xDA, 0x3A, 0xBA, 0x7A, 0xFA,
    0x06, 0x86, 0x46, 0xC6, 0x26, 0xA6, 0x66, 0xE6, 0x16, 0x96, 0x56, 0xD6, 0x36, 0xB6, 0x76, 0xF6,
    0x0E, 0x8E, 0x4E, 0xCE, 0x2E, 0xAE, 0x6E, 0xEE, 0x1E, 0x9E, 0x5E, 0xDE, 0x3E, 0xBE, 0x7E, 0xFE,
    0x01, 0x81, 0x41, 0xC1, 0x21, 0xA1, 0x61, 0xE1, 0x11, 0x91, 0x51, 0xD1, 0x31, 0xB1, 0x71, 0xF1,
    0x09, 0x89, 0x49, 0xC9, 0x29, 0xA9, 0x69, 0xE9, 0x19, 0x99, 0x59, 0xD9, 0x39, 0xB9, 0x79, 0xF9,
    0x05, 0x85, 0x45, 0xC5, 0x25, 0xA5, 0x65, 0xE5, 0x15, 0x95, 0x55, 0xD5, 0x35, 0xB5, 0x75, 0xF5,
    0x0D, 0x8D, 0x4D, 0xCD, 0x2D, 0xAD, 0x6D, 0xED, 0x1D, 0x9D, 0x5D, 0xDD, 0x3D, 0xBD, 0x7D, 0xFD,
    0x03, 0x83, 0x43, 0xC3, 0x23, 0xA3, 0x63, 0xE3, 0x13, 0x93, 0x53, 0xD3, 0x33, 0xB3, 0x73, 0xF3,
    0x0B, 0x8B, 0x4B, 0xCB, 0x2B, 0xAB, 0x6B, 0xEB, 0x1B, 0x9B, 0x5B, 0xDB, 0x3B, 0xBB, 0x7B, 0xFB,
    0x07, 0x87, 0x47, 0xC7, 0x27, 0xA7, 0x67, 0xE7, 0x17, 0x97, 0x57, 0xD7, 0x37, 0xB7, 0x77, 0xF7,
    0x0F, 0x8F, 0x4F, 0xCF, 0x2F, 0xAF, 0x6F, 0xEF, 0x1F, 0x9F, 0x5F, 0xDF, 0x3F, 0xBF, 0x7F, 0xFF
};


static uint64 TileRemapper[4][256][256];


uint32 swap(uint32 inValue)
{
    uint32 result = 0;
    
    result = result | ((inValue << 24) & (0xFF << 24));
    result = result | ((inValue <<  8) & (0xFF << 16));
    result = result | ((inValue >>  8) & (0xFF <<  8));
    result = result | ((inValue >> 24) & (0xFF <<  0));
    
    return result;
}


#include "PPU2C07.h"

void BREAKPPU()
{
  
}


PPU2C07::PPU2C07(Rom* inRom)
{
    mScanline = 241;
    mClock    = 0;
    mOddFrame = false;
    mNMILatch = false;
    memset(mVRAM, 0, sizeof(mVRAM));
    memset(mOAM, 0, sizeof(mOAM));
    
    

    mRom = inRom;
    inRom->SetVRam(mVRAM);

    mPPUCtrl = 0;
    mPPUMask = 0;
    mPPUStatus = 0;
    mOAMAddr = 0;
    
    mV = 0;
    mT = 0;
    mX = 0;
    mW = 0;
    
    for (int i = 0; i < 64; ++i)
        sNesPalette[i] = swap(sNesPalette[i]<<8);
    
    for (int a = 0; a < 4; ++a)
    {
        for (int t0 = 0; t0 < 256; t0++)
        {
            for (int t1 = 0; t1 < 256; t1++)
            {
                uint64 value = 0;
                
                for (int i = 0; i < 8; ++i)
                {
                    uint8 bt0 = (t0 >> (7-i)) & 1;
                    uint8 bt1 = (t1 >> (7-i)) & 1;
                    uint64 idx = bt0 | (bt1<<1);
                    if (idx != 0)
                        idx |= a<<2;
                    value |= idx << (i*8);
                }
                TileRemapper[a][t0][t1] = value;
            }
        }
    }
}


void PPU2C07::Tick()
{
    mClock += 3;
    
    if (mClock > 340)
    {
        if (mScanline == 240)
            mNMILatch = true;
        
        mClock -= 341;
        
        if (IsRendering())
            Scanline();
        
        if (++mScanline == 261)
            mScanline = -1;
        
        // When BG drawing is enabled, the first cycle of every odd frame is skipped
        if (mScanline == 0)
        {
            mOddFrame = !mOddFrame;
            if (mOddFrame && (mPPUMask&0x8) != 0)
                mClock++;
        }
    }
    
    if (mClock > 0)
    {
        // Time the NMI eaxactly. Also note that there is
        // one frame where reading status clears the latch
        // causing the code to miss an NMI.
        if (mNMILatch)
        {
            mNMILatch  = false;
            mPPUStatus = mPPUStatus | 0x80;
        }
        
        // Clearing NMI and sprite-0 hit happens at PPU cycle 1
        // of the pre-render scanline.
        if (mScanline == -1)
        {
            // Clear sprite hit detection and NMI
            mPPUStatus = mPPUStatus & (~0xC0);
            
            // Update mirror mode - can be set from mapper
            UpdateMirroring();
        }
    }
}



void PPU2C07::UpdateMirroring()
{
    switch (mRom->GetVRamMirroring())
    {
        case HORIZONTAL:
            mNameTable[0] = 0x2000;
            mNameTable[1] = 0x2000;
            mNameTable[2] = 0x2400;
            mNameTable[3] = 0x2400;
            break;

        case VERTICAL:
            mNameTable[0] = 0x2000;
            mNameTable[1] = 0x2400;
            mNameTable[2] = 0x2000;
            mNameTable[3] = 0x2400;
            break;
     
        case SINGLE_LOWER:
            mNameTable[0] = 0x2000;
            mNameTable[1] = 0x2000;
            mNameTable[2] = 0x2000;
            mNameTable[3] = 0x2000;
            break;

        case SINGLE_UPPER:
            mNameTable[0] = 0x2400;
            mNameTable[1] = 0x2400;
            mNameTable[2] = 0x2400;
            mNameTable[3] = 0x2400;
            break;

        case FOUR_SCREEN:
            mNameTable[0] = 0x2000;
            mNameTable[1] = 0x2400;
            mNameTable[2] = 0x2800;
            mNameTable[3] = 0x2C00;
            break;
    }
}


uint16 PPU2C07::IncreaseScrollY(uint16 inV) const
{
    // Increasing the Y scroll register updates the associated flags in mV
    // Normally, Y wraps at 29, flipping the vertical nametable bit at the
    // same time. However, if manuyally ste above 29, it will wrap at 31 leaving
    // the nametable at the old value.

    uint16 coarse_y = (inV & EScrollYCoarseMaskTgt) >> EScrollYCoarseShiftTgt;

    if (coarse_y == 29)
    {
        coarse_y = 0;
        inV ^= 0x02 << ENameTableShiftTgt;   // switch vertical nametable
    }
    else if (coarse_y == 31)
    {
        coarse_y = 0;
    }
    else
        coarse_y++;
    
    inV &= ~EScrollYCoarseMaskTgt;
    inV |= coarse_y << EScrollYCoarseShiftTgt;
    
    return inV;
}



int PPU2C07::FetchScanlineSprites(ScanlineSprite* ioSprites)
{
    // Determine sprite height
    uint8 sprite_h = (mPPUCtrl & 0x20) ? 16 : 8;

    const uint8* spr_tile = mVRAM + ((mPPUCtrl & 0x08) ? 0x1000 : 0x0000);

    uint8 num_spr_sl = 0;
    for (int i = 0; i < 64; ++i)
    {
        uint8 spr_y = mOAM[i*4];
        
        // There is a delay of one scanline when drawing sprite
        // Check spr-y and start drawing on when we passed that scanline
        if (spr_y < mScanline && mScanline <= spr_y+sprite_h)
        {
            uint8 spr_x = mOAM[i*4 + 3];

            // Sorted insert
            int idx = 0;
            for (idx = 0; idx < num_spr_sl; idx++)
                if (spr_x < ioSprites[idx].mX)
                    break;

            if (idx < num_spr_sl)
                memmove(&(ioSprites[idx+1]), &(ioSprites[idx]), (num_spr_sl-idx) * sizeof(ScanlineSprite));

            // -1 for one frame delay (see above)
            uint8 y_offset = mScanline - spr_y - 1;
            uint8 tile_idx = mOAM[i*4 + 1];
            uint8 flags    = mOAM[i*4 + 2];
            
            ioSprites[idx].mX = spr_x;
            ioSprites[idx].mPriority   = i;
            ioSprites[idx].mPalette    = flags & 0x03;
            ioSprites[idx].mForeGround = (flags & 0x20) == 0;

            // Vertical flip
            if ((flags & 0x80) != 0)
                y_offset = (sprite_h-1)-y_offset;

            const uint8* src_tile;
            if (sprite_h == 16)
            {
                // for 8x16 sprites, lowest bit select bank.
                // Therefore, top half of sprite is always on even tile
                src_tile = mVRAM + ((tile_idx & 0x01) ? 0x1000 : 0x0000);
                tile_idx = tile_idx & 0xFE;
                
                // Lower half is on next tile.
                if (y_offset >= 8)
                {
                    tile_idx++;
                    y_offset -= 8;
                }
            }
            else
                src_tile = spr_tile;
            
            ioSprites[idx].mPlane0 = src_tile[tile_idx * 16 + y_offset];
            ioSprites[idx].mPlane1 = src_tile[tile_idx * 16 + y_offset + 8];
            
            // Horizontal flip
            if ((flags & 0x40) != 0)
            {
                ioSprites[idx].mPlane0 = BitReverseTable256[ioSprites[idx].mPlane0];
                ioSprites[idx].mPlane1 = BitReverseTable256[ioSprites[idx].mPlane1];
            }
            
            num_spr_sl++;
        }
    }
    
    return num_spr_sl;
}


void PPU2C07::Scanline()
{
    // Tranfer VRam location from temp (loopy)
    if (mScanline == 0)
        mV = mT;
    
    uint16 mask = 0x041F;
    mV &= ~mask;
    mV |= mT & mask;
    
    uint8  scanline[264];
    uint64* scanlineptr = (uint64*) &scanline[0];
    uint8*  palette = mVRAM + 0x3F00;
    
    uint16 coarse_x = (mV & EScrollXCoarseMaskTgt) >> EScrollXCoarseShiftTgt;
    uint16 coarse_y = (mV & EScrollYCoarseMaskTgt) >> EScrollYCoarseShiftTgt;
    
    uint16 fine_y   = (mV & EScrollYFineMaskTgt  ) >> EScrollYFineShiftTgt;
    uint8  attr_shift = (coarse_y&2) ? 4 : 0;
    
    bool draw_bg  = (mPPUMask & 0x08) != 0;
    bool draw_spr = (mPPUMask & 0x10) != 0;

    if (draw_bg || draw_spr)
    {
        const uint8* chr_tile = mVRAM + ((mPPUCtrl & 0x10) ? 0x1000 : 0x0000);
        
        for (int slx = 0; slx < 256+mX; slx+=8)
        {
            uint16 name_table_base = mNameTable[(mV & 0xFFF) >> 10];

            // Fetch name and attr
            uint8 name =  mVRAM[name_table_base + (mV & 0x03FF)];
            uint8 attr = (mVRAM[name_table_base + 0x03C0 + ((coarse_y>>2)*8 + (coarse_x>>2))] >> (attr_shift + ((coarse_x&2) ? 2 : 0))) & 3;
            
            // Fetch pattern
            uint8 plane0 = chr_tile[(name * 16) + fine_y];
            uint8 plane1 = chr_tile[(name * 16) + fine_y + 8];
            
            *scanlineptr++ = TileRemapper[attr][plane0][plane1];
            
            // Increment coarse X
            coarse_x++;
            if (coarse_x == 32)
            {
                coarse_x = 0;
                mV ^= 0x01 << ENameTableShiftTgt;   // switch horizontal nametable
            }
            
            mV &= ~EScrollXCoarseMaskTgt;
            mV |= coarse_x << EScrollXCoarseShiftTgt;
        }

        
        // Get sprites for this scanline
        ScanlineSprite sprites_sl[64];
        uint8 num_spr_sl = FetchScanlineSprites(sprites_sl);
        uint32* framebuffer = mFrameBuffer + mScanline*256;

        int spr_index = 0;
        for (int i = 0; i < 256; ++i)
        {
            uint8 bg_color_idx = scanline[i+mX];
            uint8 color        = draw_bg ? palette[bg_color_idx] : 0;
            
            uint8 prio = 0xFF;
            for (int s = spr_index; s < num_spr_sl; ++s)
            {
                const ScanlineSprite& spr = sprites_sl[s];
                
                if (i < spr.mX)
                    break;
                
                if (i >= spr.mX + 8)
                {
                    spr_index++;
                    continue;
                }
                
                if (i >= spr.mX)
                {
                    // Shift pattern bits to get color idx
                    uint8 shift     = 7 - (i - spr.mX);
                    uint8 color_idx = ((spr.mPlane0>>shift) & 0x01) | (((spr.mPlane1 >> shift) & 0x01)<<1);
                    
                    if (color_idx != 0 && spr.mPriority < prio)
                    {
                        color_idx += spr.mPalette * 4;
                        
                        if (bg_color_idx == 0 || spr.mForeGround)
                        {
                            if (draw_spr)
                                color = palette[0x10 + color_idx];
                            prio = spr.mPriority;
                        }
                    }
                    
                    // Hit 0 detection
                    if (spr.mPriority == 0 && color_idx != 0 && bg_color_idx != 0)
                        mPPUStatus = mPPUStatus | 0x40;
                }
            }
            
            *framebuffer++ = sNesPalette[color];
        }
    }
    
    // Increment Y
    if (fine_y == 7)
    {
        fine_y = 0;
        mV = IncreaseScrollY(mV);
    }
    else
        fine_y++;
    
    // Store fine Y in V
    mV &= ~EScrollYFineMaskTgt;
    mV |= fine_y << EScrollYFineShiftTgt;
}


void PPU2C07::Load(uint16 inAddr, uint8* outValue) const
{
    uint8 idx = inAddr&0x7;
    
    switch (idx)
    {
        case 0:
            *outValue = mPPUCtrl;
            break;
            
        case 1:
            *outValue = mPPUMask;
            break;

        case 2:
            *outValue  = mPPUStatus;
            mPPUStatus = mPPUStatus & 0x7F;
            mNMILatch  = false;
            mW = 0;
            
            break;

        case 4:
            *outValue = mOAM[mOAMAddr];
            break;

        case 7:
        {
            // Reads have a delay of one
            *outValue = mPPULoadBuffer;
            
            // Read from sprite memory
            if (mV > 0x3F00)
            {
                uint16 ea = mV;
                if (ea == 0x3F10)
                    ea = 0x3f00;
                
                else if (ea >= 0x3F00)
                    ea = ea & 0x3F1F;
                
                // No delay - but load-buffer will still be set to mirrored value below
                *outValue = mVRAM[ea];
            }

            // 0x3000 - 0x3FFF is mirrored to 0x2000 - 0x2FFF
            uint16 ea = mV & 0x3FFF;
            if (ea >= 0x3000)
                ea -= 0x1000;

            // Loads from VRam - remap on right nametable
            if ((ea & 0xF000) == 0x2000)
            {
                uint8 name_table_idx = (ea >> 10) & 3;
                ea = mNameTable[name_table_idx] + (ea & 0x3FF);
            }

            mPPULoadBuffer = mVRAM[ea];
           
            // During rendering, ctrl bits are ignored and scrollY is increased - probably a timing issue
//            if (IsRendering())
//                mV = IncreaseScrollY(mV);
//            else
                mV += (mPPUCtrl&0x04) ? 32 : 1;
        }
            break;
            
        default:
            BREAKPPU();
    }
}

void PPU2C07::Store(uint16 inAddr, uint8 inValue)
{
    uint8 idx = inAddr&0x7;
    
    switch (idx)
    {
        case 0:
            // Enabling NMI during VBlank will cause an extra NMI.
            // Not thta the timing is off - probably because the CPU
            // does its writes immidiately. 
            if ((inValue & 0x80) != 0 && !GetNMI() && !IsRendering())
                mNMILatch = true;
            
            mPPUCtrl = inValue;
            
            mT = mT & ~ENameTableMaskTgt;
            mT = mT | ((inValue & ENameTableMaskSrc) << ENameTableShiftTgt);
            break;
            
        case 1:
            mPPUMask = inValue;
            break;
            
        case 2:
            // Some unit tests write here - not sure why
            break;
            
        case 3:
            mOAMAddr = inValue;
            break;

        case 4:
            mOAM[mOAMAddr++] = inValue;
            break;

        case 5:
            if (mW == 0)
            {
                // Coarse x to T
                mT = mT & ~EScrollXCoarseMaskTgt;
                mT = mT | ((inValue & EScrollXCoarseMaskSrc) >> EScrollXCoarseShiftSrc) << EScrollXCoarseShiftTgt;

                // Fine x to X
                mX = inValue & EScrollXFineMaskSrc;
                mW = 1;
            }
            else
            {
                // Coarse y to T
                mT = mT & ~EScrollYCoarseMaskTgt;
                mT = mT | ((inValue & EScrollYCoarseMaskSrc) >> EScrollYCoarseShiftSrc) << EScrollYCoarseShiftTgt;
                
                // Fine y to T
                mT = mT & ~EScrollYFineMaskTgt;
                mT = mT | ((inValue & EScrollYFineMaskSrc)) << EScrollYFineShiftTgt;
                mW = 0;
            }
        
            break;

        case 6:

            if (mW == 0)
            {
                mT = mT & ~EAddrLoMaskTgt;
                mT = mT | ((inValue & EAddrLoMaskSrc) >> EAddrLoShiftSrc) << EAddrLoShiftTgt;
                mW = 1;
            }
            else
            {
                mT = mT & ~EAddrHiMaskTgt;
                mT = mT | ((inValue & EAddrHiMaskSrc) >> EAddrHiShiftSrc) << EAddrHiShiftTgt;
                mV = mT;
                mW = 0;
            }
            break;

        case 7:
        {
            uint16 ea = mV&0x3FFF;

            if (ea >= 0x2000 && ea < 0x3F00)
            {
                // Remap 0x3000 - 0x3F00 to 0x2000 - 0x2F00
                ea = ea&0x2FFF;
                
                uint8 name_table_idx = (ea >> 10) & 3;
                ea = mNameTable[name_table_idx] + (ea & 0x3FF);
                mVRAM[ea] = inValue;
            }
            
//            if (ea == 0x3F04 || ea == 0x3F08 || ea == 0x3F0c || ea == 0x3F10 || ea == 0x3F14 || ea == 0x3F18 || ea == 0x3F1C)
            else
            {
                if (ea == 0x3F10)
                    ea = 0x3f00;
            
                else if (ea >= 0x3F00)
                    ea = ea & 0x3F1F;
                
                mVRAM[ea] = inValue;
            }
            
            // During rendering, ctrl bits are ignored and scrollY is increased - probably a timing issue
//            if (IsRendering())
//                mV = IncreaseScrollY(mV);
//          else
                mV += (mPPUCtrl&0x04) ? 32 : 1;

            break;
        }
            
        default:
            BREAKPPU();
    }
}
