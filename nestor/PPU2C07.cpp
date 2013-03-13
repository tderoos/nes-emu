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


PPU2C07::PPU2C07(const Rom* inRom)
{
    mRom = inRom;
    mScanline = 0;
    memset(mVRAM, 0, sizeof(mVRAM));
    memset(mOAM, 0, sizeof(mOAM));
    
    mPPUCtrl = 0;
    mPPUMask = 0;
    mPPUStatus = 0;
    mPPUAddr = 0;
    mPPUScroll = 0;
    mOAMAddr = 0;
    
    mPPUAddrWriteLO = false;
    
    for (int i = 0; i < 64; ++i)
        palette[i] = swap(palette[i]<<8);
}

void PPU2C07::Tick()
{
//    mPPUStatus = 0x80;
}


void PPU2C07::Scanline(uint32_t* ioFrameBuffer)
{
    if (mScanline == 0)
        mPPUStatus = mPPUStatus & (~0x40);
    
    if (mScanline < 240 && (mPPUMask & 0x08))
    {
        const uint8_t* spr_tile = ((mPPUCtrl & 0x08) ? 0x1000 : 0x0000) + mRom->GetCHRData(0);
        const uint8_t* chr = ((mPPUCtrl & 0x10) ? 0x1000 : 0x0000) + mRom->GetCHRData(0);
        
        int y     = mScanline / 8;
        int sub_y = mScanline % 8;
        
        uint16_t  addr       = (0x2000 + (mPPUCtrl&0x03) * 0x0400) + (y*32);
        uint16_t  attr_base  = (0x23c0 + (mPPUCtrl&0x03) * 0x0400) + (y>>2)*8;
        uint16_t  attr_shift = (y&2) ? 4 : 0;
        
        uint8_t sprite_h = (mPPUCtrl & 0x20) ? 16 : 8;
        
        uint32_t* fb_addr   = ioFrameBuffer + (mScanline*256);
        
        // Walk the OAM for sprites on this scanline
        struct ScanlineSprite
        {
            uint8_t mYOffset;
            uint8_t mTileIndex;
            uint8_t mFlags;
            uint8_t mX;
            uint8_t mPriority;
        };
        
        uint8_t num_spr_sl = 0;
        ScanlineSprite sprites_sl[64];
        for (int i = 0; i < 64; ++i)
        {
            uint8_t spr_y = mOAM[i*4];
            if (spr_y >= mScanline-8 && spr_y < mScanline-8+sprite_h)
            {
                uint8_t spr_x = mOAM[i*4 + 3];
                
                // Sorted insert
                int idx = 0;
                for (idx = 0; idx < num_spr_sl; idx++)
                    if (spr_x < sprites_sl[idx].mX)
                        break;
                
                if (idx < num_spr_sl)
                    memmove(&(sprites_sl[idx+1]), &(sprites_sl[idx]), (num_spr_sl-idx) * sizeof(ScanlineSprite));
                
                sprites_sl[idx].mYOffset = spr_y - (mScanline-8);
                sprites_sl[idx].mTileIndex = mOAM[i*4 + 1];;
                sprites_sl[idx].mFlags = mOAM[i*4 + 2];;
                sprites_sl[idx].mX = spr_x;
                sprites_sl[idx].mPriority = i;

                if ((sprites_sl[idx].mFlags & 0x80) == 0)
                    sprites_sl[idx].mYOffset = 7-sprites_sl[idx].mYOffset;
                
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
                        // Fetch pattern
                        uint8_t plane0 = spr_tile[(spr.mTileIndex * 16) + spr.mYOffset];
                        uint8_t plane1 = spr_tile[(spr.mTileIndex * 16) + spr.mYOffset + 8];
                        
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
                            if (prio == 0 /*&& bg_color_idx != 0*/)
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

    if (mScanline == 241)
        mPPUStatus = mPPUStatus | 0x80;
    
    mScanline = (mScanline+1) % 260;
}


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

                // Map low addresses on chr data
                if (mPPUAddr < 0x2000)
                    mPPULoadBuffer = mRom->GetCHRData(0)[mPPUAddr];

                else
                    mPPULoadBuffer = mVRAM[mPPUAddr & 0x3FFF];
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
            break;

        case 6:
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

            if (mPPUAddr >= 0x3320 && mPPUAddr <= 0x3FFF)
                mPPUAddr = mPPUAddr;
            
            if (ea >= 0x2000 && ea < 0x3F00)
                ea = ea & 0x2FFF;
            
//            if (ea == 0x3F04 || ea == 0x3F08 || ea == 0x3F0c || ea == 0x3F10 || ea == 0x3F14 || ea == 0x3F18 || ea == 0x3F1C)
            if (ea == 0x3F10)
                ea = 0x3f00;
            
            else if (ea >= 0x3F00)
                ea = ea & 0x3F1F;
            
            if (ea != mPPUAddr)
                mPPUAddr = mPPUAddr;
            
            mVRAM[ea] = inValue;

            mPPUAddr += (mPPUCtrl&0x04) ? 32 : 1;
            break;
        }
            
        default:
            BREAKPPU();
    }
}
