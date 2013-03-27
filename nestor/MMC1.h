//
//  MMC1.h
//  nestor
//
//  Created by Tommy de Roos on 3/12/13.
//
//

#ifndef nestor_MMC1_h
#define nestor_MMC1_h

#include "Mapper.h"
#include <string.h>

class MMC1 : public Mapper
{
public:
    MMC1(uint8 inNumPRG, uint8 inNumCHR) : Mapper(inNumPRG, inNumCHR)
    {
        mShiftIndex = 0;
        mTempReg    = 0;
        mControl    = 0x0C;
        mCHRReg0    = 0;
        mCHRReg1    = 0;
        mPRGReg     = 0;
    }

    virtual void Store(uint16 inAddr, uint8 inData)
    {
        // MMC1 uses a serial register write to update the mapping
        // Registers a 5 bit wide, addres at write of the 5th bit
        // determines tgt register. b0 of value is data, b7 is reset
        
        // if not reset
        if ((inData & 0x80) == 0)
        {
            mTempReg = mTempReg | ((inData & 0x01) << mShiftIndex++);
            
            if (mShiftIndex == 5)
            {
                int idx = (inAddr >> 13) & 0x3;

                mDirty = mDirty || (mRegisters[idx] != mTempReg);
                mRegisters[idx] = mTempReg;
                mShiftIndex = 0;
                mTempReg = 0;
            }
        }
        else
        {
            mShiftIndex = 0;
            mTempReg = 0;
            
            mDirty = mDirty || (mControl != 0x0C);

            mControl |= 0x0C;
        }
    }
    
    virtual void UpdateMapping(const uint8* inData, uint8* ioPRG, uint8* ioCHR, EVRamLayout inRomLayout, EVRamLayout* outMappedLayout) const
    {
        // Map PRG data
        uint8 bank_idx = mPRGReg&0x0F;
        switch((mControl >> 2) & 0x03)
        {
            case 0:
            case 1:
                memcpy(ioPRG, inData + (bank_idx&0x0E) * 0x4000, 0x8000);
                break;
                
            case 2:
                memcpy(ioPRG,          inData                    , 0x4000);
                memcpy(ioPRG + 0x4000, inData + bank_idx * 0x4000, 0x4000);
                break;
                
            case 3:
                memcpy(ioPRG,          inData +    bank_idx * 0x4000, 0x4000);
                memcpy(ioPRG + 0x4000, inData + (mNumPRG-1) * 0x4000, 0x4000);
                break;
        }
        
        // Map CHR data
        if (mNumCHR != 0)
        {
            const uint8* chr_start = inData + mNumPRG * 0x4000;
            if ((mControl & 0x10) == 0)
            {
                // 8K Mode
                memcpy(ioCHR, chr_start + (mCHRReg0&0xE) * 0x1000, 0x2000);
            }
            else
            {
                // 4K Mode
                memcpy(ioCHR,          chr_start + mCHRReg0 * 0x1000, 0x1000);
                memcpy(ioCHR + 0x1000, chr_start + mCHRReg1 * 0x1000, 0x1000);
            }
        }
        
        if (inRomLayout != FOUR_SCREEN)
        {
            switch(mControl&0x03)
            {
                case 0: *outMappedLayout = SINGLE_LOWER; break;
                case 1: *outMappedLayout = SINGLE_UPPER; break;
                case 2: *outMappedLayout = VERTICAL;     break;
                case 3: *outMappedLayout = HORIZONTAL;   break;
            }
        }
        else
            *outMappedLayout = inRomLayout;
        
        mDirty = false;
    }

private:
    uint8   mShiftIndex;
    uint8   mTempReg;

    union {
        struct {
            uint8   mControl;       // $8000
            uint8   mCHRReg0;       // $A000 - CHR select 0
            uint8   mCHRReg1;       // $C000 - CHR select 1
            uint8   mPRGReg;        // $E000 - PRG select / WRAM enable
        };
        uint8  mRegisters[4];
    };
    
};

#endif
