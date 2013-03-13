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
    MMC1(UInt8 inNumPRG, UInt8 inNumCHR) : Mapper(inNumPRG, inNumCHR)
    {
        mShiftIndex = 0;
        mTempReg    = 0;
        mControl    = 0x0C;
        mCHRReg0    = 0;
        mCHRReg1    = 0;
        mPRGReg     = 0;
    }

    virtual void Store(UInt16 inAddr, UInt8 inData)
    {
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
    
    virtual void UpdateMapping(const UInt8* inData, UInt8* ioPRG, UInt8* ioCHR) const
    {
        // Map PRG data
        UInt8 bank_idx = mPRGReg&0x0F;
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
        const UInt8* chr_start = inData + mNumPRG * 0x4000;
        if ((mControl & 0x10) == 0)
        {
            // 8K Mode
            memcpy(ioCHR, chr_start + mCHRReg0 * 0x1000, 0x2000);
        }
        else
        {
            // 4K Mode
            memcpy(ioCHR,          chr_start + mCHRReg0 * 0x1000, 0x1000);
            memcpy(ioCHR + 0x1000, chr_start + mCHRReg1 * 0x1000, 0x1000);
        }
        
        mDirty = false;
    }

private:
    UInt8   mShiftIndex;
    UInt8   mTempReg;

    union {
        struct {
            UInt8   mControl;       // $8000
            UInt8   mCHRReg0;       // $A000 - CHR select 0
            UInt8   mCHRReg1;       // $C000 - CHR select 1
            UInt8   mPRGReg;        // $E000 - PRG select / WRAM enable
        };
        UInt8  mRegisters[4];
    };
    
};

#endif
