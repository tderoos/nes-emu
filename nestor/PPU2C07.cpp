//
// Created by tderoos on 3/3/13.
//
// To change the template use AppCode | Preferences | File Templates.
//

#include <stdint.h>

/*
const uint32_t palette[64] = {
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
unsigned int palette[] =
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

static const unsigned char BitReverseTable256[] =
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



uint32_t swap(uint32_t inValue)
{
    uint32_t result = 0;
    
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
    mScanline = 0;
    memset(mVRAM, 0, sizeof(mVRAM));
    memset(mOAM, 0, sizeof(mOAM));

    mRom = inRom;
    inRom->SetVRam(mVRAM);

    mPPUCtrl = 0;
    mPPUMask = 0;
    mPPUStatus = 0;
    mPPUAddr = 0;
    mPPUScroll = 0;
    mOAMAddr = 0;
    
    mV = 0;
    mT = 0;
    mX = 0;
    mW = 0;
    
    mPPUAddrWriteLO = false;
    
    for (int i = 0; i < 64; ++i)
        palette[i] = swap(palette[i]<<8);
}



void PPU2C07::UpdateMirroring()
{
    switch (mRom->GetVRamMirroring())
    {
        case VERTICAL:
            mNameTable[0] = 0x2000;
            mNameTable[1] = 0x2000;
            mNameTable[2] = 0x2400;
            mNameTable[3] = 0x2400;
            break;

        case HORIZONTAL:
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


int PPU2C07::FetchScanlineSprites(ScanlineSprite* ioSprites)
{
    // Determine sprite height
    UInt8 sprite_h = (mPPUCtrl & 0x20) ? 16 : 8;

    const UInt8* spr_tile = mVRAM + ((mPPUCtrl & 0x08) ? 0x1000 : 0x0000);

    UInt8 num_spr_sl = 0;
    for (int i = 0; i < 64; ++i)
    {
        uint8_t spr_y = mOAM[i*4];
        
        if (spr_y+sprite_h >= mScanline && spr_y < mScanline)
        {
            uint8_t spr_x = mOAM[i*4 + 3];

            // Sorted insert
            int idx = 0;
            for (idx = 0; idx < num_spr_sl; idx++)
                if (spr_x < ioSprites[idx].mX)
                    break;

            if (idx < num_spr_sl)
                memmove(&(ioSprites[idx+1]), &(ioSprites[idx]), (num_spr_sl-idx) * sizeof(ScanlineSprite));

            UInt8 y_offset = spr_y - (mScanline-sprite_h);
            UInt8 tile_idx = mOAM[i*4 + 1];
            UInt8 flags    = mOAM[i*4 + 2];
            
            ioSprites[idx].mX = spr_x;
            ioSprites[idx].mPriority   = i;
            ioSprites[idx].mPalette    = flags & 0x03;
            ioSprites[idx].mForeGround = (flags & 0x20) == 0;
            
            if ((flags & 0x80) == 0)
                y_offset = (sprite_h-1)-y_offset;

            const UInt8* src_tile;
            if (sprite_h == 16)
            {
                src_tile = mVRAM + ((tile_idx & 0x01) ? 0x1000 : 0x0000);
                tile_idx = tile_idx & 0xFE;
            }
            else
                src_tile = spr_tile;
            
            ioSprites[idx].mPlane0 = src_tile[tile_idx * 16 + y_offset];
            ioSprites[idx].mPlane1 = src_tile[tile_idx * 16 + y_offset + 8];
            
            if ((flags & 0x40) == 0)
            {
                ioSprites[idx].mPlane0 = BitReverseTable256[ioSprites[idx].mPlane0];
                ioSprites[idx].mPlane1 = BitReverseTable256[ioSprites[idx].mPlane1];
            }
            
            num_spr_sl++;
        }
    }
    
    return num_spr_sl;
}


void PPU2C07::Scanline(uint32_t* ioFrameBuffer)
{
    if (mScanline == 0)
    {
        mPPUStatus = mPPUStatus & (~0x40);
        UpdateMirroring();
    }

    
#if 1
    
    UInt16 fine_y   = (mV & EScrollYFineMaskTgt  ) >> EScrollYFineShiftTgt;

    if (mScanline < 240 && (mPPUMask & 0x08))
    {
        // Get sprites for this scanline
        ScanlineSprite sprites_sl[64];
        UInt8 num_spr_sl = FetchScanlineSprites(sprites_sl);
        
        UInt32*      fb_addr  = ioFrameBuffer + (mScanline*256);
        const UInt8* chr_tile = mVRAM + ((mPPUCtrl & 0x10) ? 0x1000 : 0x0000);
        
        UInt8 base_nametable = mPPUCtrl & 0x3;
        
        // Determine Y
        int scrolled_y = mScanline + mPPUScroll & 0xFF;
        int tile_y     = scrolled_y / 8;
        int sub_y      = scrolled_y % 8;
        UInt8  attr_shift = (tile_y&2) ? 4 : 0;

        // Clamp tile_y and adjust nametable if tile_y out of bounds.
        if (tile_y >= 29)
        {
            base_nametable = base_nametable ^ 0x02;
            tile_y -= 29;
        }

        // Determine X
        UInt8 scroll_x = mPPUScroll >> 8;
        int from_x = scroll_x & 7;
        int tile_x = scroll_x >> 3;
        
        int spr_index = 0;
        
        if (mScanline == 0)
            mV = mT;
        else
        {
            UInt16 mask = 0x041F;
            mV &= ~mask;
            mV |= mT & mask;
        }
//        sub_y = 0;

        fine_y   = (mV & EScrollYFineMaskTgt  ) >> EScrollYFineShiftTgt;
        for (int slx = 0; slx < 256; )
        {
            // Clamp tile_x and adjust nametable if tile_x out of bounds.
            base_nametable = base_nametable ^ (tile_x >> 5);
            tile_x         = tile_x & 0x1F;

            // Get nametable address
            UInt8* name_table_old = mVRAM + mNameTable[base_nametable];
            UInt8* attr_table_old = name_table_old + 0x3c0;
            
            UInt8 nti = (mV & 0xFFF) >> 10;
            
            UInt16 name_table_addr = (mNameTable[nti] + (mV & 0x03FF));
//            UInt8* name_table = mVRAM + (mNameTable[nti] + (mV & 0x03FF));
//            UInt8* attr_table = mVRAM + (0x23C0 | (mV & 0x0C00) | ((mV >> 4) & 0x38) | ((mV >> 2) & 0x07));
            
            // Fetch name and attr
//            UInt8 name = *name_table;//   name_table[tile_x      + tile_y*32    ];//
//            UInt8 attr = *attr_table;//(attr_table[(tile_x>>2) + (tile_y>>2)*8] >> (attr_shift + ((tile_x&2) ? 2 : 0))) & 3;

            UInt8 name = mVRAM[name_table_addr];
            
            // Fetch pattern
            UInt8 plane0 = chr_tile[(name * 16) + fine_y];
            UInt8 plane1 = chr_tile[(name * 16) + fine_y + 8];
            
//            for (int tx = from_x; tx < 8 && slx < 256; ++tx, ++slx)
            UInt8 tx = mX;
            {
                uint8_t color = 0;
                
                // BG
                uint8_t bit0 = (plane0 >> (7-tx)) & 1;
                uint8_t bit1 = (plane1 >> (7-tx)) & 1;
                uint8_t bg_color_idx = bit0 | (bit1<<1) ;

//                if (bg_color_idx != 0)
//                    bg_color_idx |= (attr<<2);
                
                if (mPPUMask & 0x08)
                    color = mVRAM[0x3F00 + bg_color_idx];

                uint8_t prio = 0xFF;
                for (int s = spr_index; s < num_spr_sl; ++s)
                {
                    const ScanlineSprite& spr = sprites_sl[s];
                    
                    if (slx < spr.mX)
                        break;

                    if (slx >= spr.mX + 8)
                    {
                        spr_index++;
                        continue;
                    }
                    
                    if (slx >= spr.mX)
                    {
                        // Shift pattern bits to get color idx
                        uint8_t shift     = slx - spr.mX;
                        uint8_t color_idx = ((spr.mPlane0>>shift) & 0x01) | (((spr.mPlane1 >> shift) & 0x01)<<1);
                        
                        if (color_idx != 0 && spr.mPriority < prio)
                        {
                            color_idx += spr.mPalette * 4;
                            
                            if (bg_color_idx == 0 || spr.mForeGround)
                            {
                                if ((mPPUMask & 0x10) != 0)
                                    color = mVRAM[0x3F10 + color_idx];
                                prio = spr.mPriority;
                            }
                            
                            if (spr.mPriority == 0)// && bg_color_idx != 0)
                                mPPUStatus = mPPUStatus | 0x40;
                        }
//
//                        if (spr.mPriority == 0 && color_idx != 0 && bg_color_idx != 0)
//                            mPPUStatus = mPPUStatus | 0x40;
                        
                    }
                }
                slx++;
                
                (*fb_addr++) = palette[color];
                
                // Incerement X
                if (mX == 7)
                {
                    // Increment coarse X
                    UInt16 coarse_x = (mV & EScrollXCoarseMaskTgt) >> EScrollXCoarseShiftTgt;
                    coarse_x++;
                    if (coarse_x == 32)
                    {
                        coarse_x = 0;
                        mV ^= 0x01 << ENameTableShiftTgt;   // switch horizontal nametable
                    }
                    
                    mV &= ~EScrollXCoarseMaskTgt;
                    mV |= coarse_x << EScrollXCoarseShiftTgt;

                    mX = 0;
                }
                else
                    mX++;
            }
            
            // Reset x for next iteration
            from_x = 0;
            tile_x++;
        }
    }
    else if (mScanline == 241)
        mPPUStatus = mPPUStatus | 0x80;
  
    // Increment Y
    UInt16 coarse_y = (mV & EScrollYCoarseMaskTgt) >> EScrollYCoarseShiftTgt;
    if (fine_y ==7)
    {
        fine_y = 0;
        
        if (coarse_y == 29)
        {
            coarse_y = 0;
            mV ^= 0x02 << ENameTableShiftTgt;   // switch vertical nametable
        }
        else if (coarse_y == 31)
        {
            coarse_y = 0;
        }
        else
            coarse_y++;
    }
    else
        fine_y++;
    
    // Store Y in V
    mV &= ~(EScrollYCoarseMaskTgt | EScrollYFineMaskTgt);
    mV |= (coarse_y << EScrollYCoarseShiftTgt) | (fine_y << EScrollYFineShiftTgt);

//    if (mScanline == 261)
//    {
//        mV = mT;
//    }
//    else if (mScanline > 261)
//    {
//        UInt16 mask = 0x041F;
//        mV &= ~mask;
//        mV |= mT & mask;
//    }
    
    
    mScanline = (mScanline+1) % 262;
}


#else


    if (mScanline < 240 && (mPPUMask & 0x08))
    {
        UInt32*      fb_addr  = ioFrameBuffer + (mScanline*256);

        const UInt8* spr_tile = mVRAM + ((mPPUCtrl & 0x08) ? 0x1000 : 0x0000);
        const UInt8* chr      = mVRAM + ((mPPUCtrl & 0x10) ? 0x1000 : 0x0000);
        
        int effective_sl = (mScanline + (mPPUScroll & 0xFF)) % 240;
        
        int y     = effective_sl / 8;
        int sub_y = effective_sl % 8;
        
        uint16_t  addr       = (0x2000 + (mPPUCtrl&0x03) * 0x0400) + (y*32);
        uint16_t  attr_base  = (0x23c0 + (mPPUCtrl&0x03) * 0x0400) + (y>>2)*8;
        uint16_t  attr_shift = (y&2) ? 4 : 0;
        
        uint8_t sprite_h = (mPPUCtrl & 0x20) ? 16 : 8;
        
        
        // Walk the OAM for sprites on this scanline
        struct ScanlineSprite
        {
            UInt8 mYOffset;
            UInt8 mTileIndex;
            UInt8 mFlags;
            UInt8 mX;
            UInt8 mPriority;
            const UInt8* mSrc;
        };
        
        uint8_t num_spr_sl = 0;
        ScanlineSprite sprites_sl[64];
        for (int i = 0; i < 64; ++i)
        {
            uint8_t spr_y = mOAM[i*4];
            if (spr_y+sprite_h >= effective_sl && spr_y < effective_sl)
            {
                uint8_t spr_x = mOAM[i*4 + 3];
                
                // Sorted insert
                int idx = 0;
                for (idx = 0; idx < num_spr_sl; idx++)
                    if (spr_x < sprites_sl[idx].mX)
                        break;
                
                if (idx < num_spr_sl)
                    memmove(&(sprites_sl[idx+1]), &(sprites_sl[idx]), (num_spr_sl-idx) * sizeof(ScanlineSprite));
                
                sprites_sl[idx].mYOffset = spr_y - (effective_sl-sprite_h);
                sprites_sl[idx].mTileIndex = mOAM[i*4 + 1];
                sprites_sl[idx].mFlags = mOAM[i*4 + 2];
                sprites_sl[idx].mX = spr_x;
                sprites_sl[idx].mPriority = i;

                if ((sprites_sl[idx].mFlags & 0x80) == 0)
                    sprites_sl[idx].mYOffset = sprite_h-1-sprites_sl[idx].mYOffset;
                
                if (sprite_h == 0x10)
                {
                    UInt8 tile_idx = sprites_sl[idx].mTileIndex;
                    sprites_sl[idx].mSrc = mVRAM + ((tile_idx & 0x01) ? 0x1000 : 0x0000);
                    sprites_sl[idx].mTileIndex = tile_idx & 0xFE;
                }
                else
                    sprites_sl[idx].mSrc = spr_tile;
                
                num_spr_sl++;
            }
        }
        
        uint8_t spr_index = 0;
        
        uint8_t scroll_x = mPPUScroll >> 8;
        uint8_t tile_scroll_x = scroll_x >> 3;
        uint8_t fine_scroll_x = scroll_x & 7;
  
        for (int draw_x = 0; draw_x < 32; ++draw_x)
        {
            uint8_t tile_x = tile_scroll_x + draw_x;
            uint16_t addr_xor = 0;
            if (tile_x >= 32)
            {
                addr_xor = 0x0400;
                tile_x -= 32;
            }
            
            uint8_t name = mVRAM[(addr^addr_xor) + tile_x];
            uint8_t attr = (mVRAM[(attr_base^addr_xor) + (tile_x/4)] >> (attr_shift + ((tile_x&2) ? 2 : 0))) & 3;
            
            // Fetch pattern
            uint8_t plane0 = chr[(name * 16) + sub_y];
            uint8_t plane1 = chr[(name * 16) + sub_y + 8];
            
            const uint8_t from_x = (draw_x == 0  ?   fine_scroll_x : 0);
            const uint8_t   to_x = (draw_x == 31 ? 8-fine_scroll_x : 8);
            
            for (int sx = from_x; sx < to_x; ++sx)
            {
                uint8_t color = 0;
                
                // BG
                uint8_t bit0 = (plane0 >> (7-sx)) & 1;
                uint8_t bit1 = (plane1 >> (7-sx)) & 1;
                uint8_t bg_color_idx = bit0 | (bit1<<1) ;
                
                if (bg_color_idx != 0)
                    bg_color_idx |= (attr<<2);
                
                if (mPPUMask & 0x08)
                    color = mVRAM[0x3F00 + bg_color_idx];

                // Sprites
                uint8_t x_pos = (draw_x*8) + sx - fine_scroll_x;
                uint8_t prio = 0x0;

                for (int s = spr_index; s < num_spr_sl; ++s)
                {
                    const ScanlineSprite& spr = sprites_sl[s];
                    
                    if (x_pos >= spr.mX + 8)
                    {
                        spr_index++;
                        continue;
                    }
                    
                    if (x_pos >= spr.mX)
                    {
                        if (spr.mPriority==0)
                            s = s;
                        
                        // Fetch pattern
                        uint8_t plane0 = spr.mSrc[(spr.mTileIndex * 16) + spr.mYOffset];
                        uint8_t plane1 = spr.mSrc[(spr.mTileIndex * 16) + spr.mYOffset + 8];
                        
                        uint8_t shift = x_pos - spr.mX;
                        
                        if ((spr.mFlags&0x40) == 0)
                            shift = 7 - shift;
                        
                        uint8_t color_idx = ((plane0>>shift) & 0x01) | (((plane1 >> shift) & 0x01)<<1);

                        if (color_idx != 0 && spr.mPriority >= prio)
                        {
                            color_idx += (spr.mFlags & 0x03) * 4;
                            
                            if (bg_color_idx == 0 || (spr.mFlags & 0x20) == 0)
                            {
                                if ((mPPUMask & 0x10) != 0)
                                    color = mVRAM[0x3F10 + color_idx];
                                prio = spr.mPriority;
                            }
                            if (prio == 0) //&& bg_color_idx != 0)
                                mPPUStatus = mPPUStatus | 0x40;
                        }
//                        
//                        if (spr.mPriority == 0 && color_idx != 0 && bg_color_idx != 0)
//                            mPPUStatus = mPPUStatus | 0x40;

                    }
                }
                
                (*fb_addr++) = palette[color];
            }
        }
    }

    else if (mScanline == 241)
        mPPUStatus = mPPUStatus | 0x80;
    
    mScanline = (mScanline+1) % 262;
}


#endif

void PPU2C07::Load(uint16_t inAddr, uint8_t* outValue) const
{
    uint8_t idx = inAddr&0x7;
    
    switch (idx)
    {
        case 0:
            *outValue = mPPUCtrl;
            break;
            
        case 1:
            *outValue = mPPUMask;
            break;

        case 2:
            *outValue = mPPUStatus;
            mPPUStatus = mPPUStatus & 0x7F;
//            mPPUScroll = 0;
            mPPUAddr = 0;
            mPPUAddrWriteLO = false;
            
            mW = 0;
            
            break;

        case 4:
            *outValue = mOAM[mOAMAddr++];
            break;

        case 7:
            
            if (mPPUAddr > 0x3F00)
            {
                *outValue = mVRAM[mPPUAddr & 0x3FFF];
            }
            else
            {
                // Reads have a delay of one
                *outValue = mPPULoadBuffer;
                
                UInt16 ea = mPPUAddr & 0x3FFF;
                if ((ea & 0xF000) == 0x2000)
//                if (ea >= 0x2000 && ea < 0x3000)
                {
                    UInt8 name_table_idx = (ea >> 10) & 3;
                    ea = mNameTable[name_table_idx] + (ea & 0x3FF);
                }

                mPPULoadBuffer = mVRAM[ea];
            }

            mPPUAddr += (mPPUCtrl&0x04) ? 32 : 1;
            break;
            
        default:
            BREAKPPU();
    }
}

void PPU2C07::Store(uint16_t inAddr, uint8_t inValue)
{
    uint8_t idx = inAddr&0x7;
    
    switch (idx)
    {
        case 0:
            mPPUCtrl = inValue;
            
            mT = mT & ~ENameTableMaskTgt;
            mT = mT | ((inValue & ENameTableMaskSrc) << ENameTableShiftTgt);
            break;
            
        case 1:
            mPPUMask = inValue;
            break;
            
        case 3:
            mOAMAddr = inValue;
            break;

        case 4:
            mOAM[mOAMAddr++] = inValue;
            break;

        case 5:
            mPPUScroll = (mPPUScroll<<8) | inValue;
            
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
            
            if (mPPUAddrWriteLO)
                mPPUAddr = (mPPUAddr & 0xFF00) | inValue;

            else
                mPPUAddr = (mPPUAddr & 0x00FF) | inValue << 8;
            
//            mPPUAddr = (mPPUAddr<<8) | (inValue);
            mPPUAddrWriteLO = !mPPUAddrWriteLO;
            break;

        case 7:
        {
            uint16_t ea = mPPUAddr&0x3FFF;

            if ((ea & 0xF000) == 0x2000)
            {
                UInt8 name_table_idx = (ea >> 10) & 3;
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
            
            mPPUAddr += (mPPUCtrl&0x04) ? 32 : 1;
            break;
        }
            
        default:
            BREAKPPU();
    }
}
