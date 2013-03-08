//
// Created by tderoos on 3/3/13.
//
// To change the template use AppCode | Preferences | File Templates.
//


#include "PPU2C07.h"

void BREAKPPU()
{
    
}


PPU2C07::PPU2C07()
{
    mScanline = 0;
}

void PPU2C07::Tick()
{
//    mPPUStatus = 0x80;
}


void PPU2C07::Scanline()
{
    static bool render = true;
    if (render && mScanline == 0)
    {
        uint16_t addr = 0x2000 + (mPPUStatus&0x03) * 0x0400;
        
        for (int y = 0; y < 30; ++y)
        {
            for (int x = 0; x < 32; ++x)
                printf("%c", mVRAM[addr++]);
            printf("\n");
        }
    }
    
    if (mScanline < 240)
    {
        
    }
    
    if (mScanline == 241)
        mPPUStatus = mPPUStatus | 0x80;
    
    mScanline = mScanline+1 % 260;
}


void PPU2C07::Load(uint16_t inAddr, uint8_t* outValue) const
{
    uint8_t idx = inAddr&0x7;
    
    switch (idx)
    {
        case 2:
            *outValue = mPPUStatus;
            mPPUStatus = mPPUStatus & 0x7F;
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
            mPPUAddr += (mPPUStatus&0x04) ? 32 : 1;
            break;
            
        default:
            BREAKPPU();
    }
}
