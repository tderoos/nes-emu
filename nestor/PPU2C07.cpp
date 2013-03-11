//
// Created by tderoos on 3/3/13.
//
// To change the template use AppCode | Preferences | File Templates.
//


unsigned int palette[] =
{
    0x7C7C7CFF,
    0x0000FCFF,
    0x0000BCFF,
    0x4428BCFF,
    0x940084FF,
    0xA80020FF,
    0xA81000FF,
    0x881400FF,
    0x503000FF,
    0x007800FF,
    0x006800FF,
    0x005800FF,
    0x004058FF,
    0x000000FF,
    0x000000FF,
    0x000000FF,

    0xBCBCBCFF,
    0x0078F8FF,
    0x0058F8FF,
    0x6844FCFF,
    0xD800CCFF,
    0xE40058FF,
    0xF83800FF,
    0xE45C10FF,
    0xAC7C00FF,
    0x00B800FF,
    0x00A800FF,
    0x00A844FF,
    0x008888FF,
    0x000000FF,
    0x000000FF,
    0x000000FF,
   
    0xF8F8F8FF,
    0x3CBCFCFF,
    0x6888FCFF,
    0x9878F8FF,
    0xF878F8FF,
    0xF85898FF,
    0xF87858FF,
    0xFCA044FF,
    0xF8B800FF,
    0xB8F818FF,
    0x58D854FF,
    0x58F898FF,
    0x00E8D8FF,
    0x787878FF,
    0x000000FF,
    0x000000FF,

    0xFCFCFCFF,
    0xA4E4FCFF,
    0xB8B8F8FF,
    0xD8B8F8FF,
    0xF8B8F8FF,
    0xF8A4C0FF,
    0xF0D0B0FF,
    0xFCE0A8FF,
    0xF8D878FF,
    0xD8F878FF,
    0xB8F8B8FF,
    0xB8F8D8FF,
    0x00FCFCFF,
    0xF8D8F8FF,
    0x000000FF,
    0x000000FF
};

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
}

void PPU2C07::Tick()
{
//    mPPUStatus = 0x80;
}


uint32_t swap(uint32_t inValue)
{
    uint32_t result = 0;
    
    result = result | ((inValue << 24) & (0xFF << 24));
    result = result | ((inValue <<  8) & (0xFF << 16));
    result = result | ((inValue >>  8) & (0xFF <<  8));
    result = result | ((inValue >> 24) & (0xFF <<  0));
    
    return result;
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
                    bg_color_idx += 4*attr;
                
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
                
                (*fb_addr++) = swap(palette[color]);
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
        case 2:
            *outValue = mPPUStatus;
            mPPUStatus = mPPUStatus & 0x7F;
//            mPPUScroll = 0;
            mPPUAddr = 0;
            break;

        case 4:
            *outValue = mOAM[mOAMAddr++];
            break;

        case 7:
            *outValue = mVRAM[mPPUAddr++];
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
//            printf("OAMADDR : 0x%x\n", mOAMAddr);
            break;

        case 4:
            mOAM[mOAMAddr] = inValue;
//            printf("AOM[0x%x] = $%x\n", mOAMAddr, inValue);
            mOAMAddr++;
            break;

        case 5:
            mPPUScroll = (mPPUScroll<<8) | inValue;
//            printf("PPUSCROLL : 0x%x\n", mPPUScroll);
            break;

        case 6:
            mPPUAddr = (mPPUAddr<<8) | (inValue);
//            printf("PPUADDR : 0x%x\n", mPPUAddr);
            break;
            
        case 7:
            mVRAM[mPPUAddr & 0x3FFF] = inValue;
//            printf("VRAM[0x%x] = $%x\n", mPPUAddr, inValue);
            mPPUAddr += (mPPUCtrl&0x04) ? 32 : 1;
            break;
            
        default:
            BREAKPPU();
    }
}
