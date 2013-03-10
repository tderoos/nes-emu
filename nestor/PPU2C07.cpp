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
    mPPUStatus = mPPUStatus & (~0x40);
    
    if (mScanline < 240 && (mPPUMask & 0x08))
    {
        const uint8_t* spr = ((mPPUCtrl & 0x80) ? 0x1000 : 0x0000) + mRom->GetCHRData(0);
        const uint8_t* chr = ((mPPUCtrl & 0x10) ? 0x1000 : 0x0000) + mRom->GetCHRData(0);
        
        int y     = mScanline / 8;
        int sub_y = mScanline % 8;
        
        uint16_t  addr       = (0x2000 + (mPPUStatus&0x03) * 0x0400) + (y*32);
        uint16_t  attr_base  = (0x23c0 + (mPPUStatus&0x03) * 0x0400) + (y>>2)*8;
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
        };
        
        uint8_t num_spr_sl = 0;
        ScanlineSprite sprites_sl[64];
        for (int i = 0; i < 64; ++i)
        {
            uint8_t spr_y = mOAM[i*4];
            if (spr_y >= mScanline && spr_y < mScanline+sprite_h)
            {
                uint8_t spr_x = mOAM[i*4 + 3];
                
                // Sorted insert
                int idx = 0;
                for (idx = 0; idx < num_spr_sl; idx++)
                    if (spr_x < sprites_sl[idx].mX)
                        break;
                
                if (idx < num_spr_sl)
                    memmove(&sprites_sl[idx+1], &sprites_sl[idx], num_spr_sl-idx * sizeof(uint8_t));
                
                sprites_sl[idx].mYOffset = spr_y - mScanline;
                sprites_sl[idx].mTileIndex = mOAM[i*4 + 1];;
                sprites_sl[idx].mFlags = mOAM[i*4 + 2];;
                sprites_sl[idx].mX = spr_x;

                num_spr_sl++;
            }
        }
        
        uint8_t spr_index = 0;
        
        uint8_t scroll_x = mPPUScroll >> 8;
        uint8_t tile_scroll_x = scroll_x >> 3;
        uint8_t fine_scroll_x = scroll_x & 7;
  
        for (int x = 0; x < 32; ++x)
        {
            uint8_t name = mVRAM[addr++];
            uint8_t attr = (mVRAM[attr_base + (x/4)] >> (attr_shift + ((x&2) ? 2 : 0))) & 3;
            
            uint8_t num_spr_tl = 0;
            uint8_t x_start = x*8, x_end = x_start+8;
            for (int s = spr_index; s < num_spr_sl; ++s)
            {
                const ScanlineSprite& spr = sprites_sl[s];
                
                if (spr.mX+8 < x_start)
                {
                    spr_index++;
                    continue;
                }
                
                if (spr.mX > x_end)
                    break;
                
                num_spr_tl++;
            }
            
//            name = y*32+x;

            // Fetch pattern
            uint8_t plane0 = chr[(name * 16) + sub_y];
            uint8_t plane1 = chr[(name * 16) + sub_y + 8];
            
            for (int i = 7; i >= 0; --i)
            {
                uint8_t x_pos = x*8 + (7-i);
                uint8_t spr_color = 0;
                for (int s = 0; s < num_spr_tl; ++s)
                {
                    const ScanlineSprite& spr = sprites_sl[spr_index+s];
                    if (x_pos >= spr.mX && x_pos < spr.mX+8)
                    {
                        spr_color = 1;
                        mPPUStatus = mPPUStatus | 0x40;
                    }
                }
                
                uint8_t bit0 = (plane0 >> i) & 1;
                uint8_t bit1 = (plane1 >> i) & 1;
                uint8_t color_idx = bit0 | (bit1<<1) ;//| (attr << 2);
                
                if (color_idx != 0)
                    color_idx += 4*attr;
                else
                    color_idx = color_idx;
//                uint8_t color_idx = (bit0 | (bit1<<1)) + (16*attr);
                
                uint8_t color = mVRAM[0x3F00 + color_idx];
                (*fb_addr++) = spr_color ? 0xFFFFFFFF : swap(palette[color]);
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
            mPPUScroll = 0;
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
