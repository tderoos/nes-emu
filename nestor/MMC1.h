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

class MMC1 : public Mapper
{
public:
    MMC1(UInt8 inNumPRG, UInt8 inNumCHR)
    {
        mNumPRG = inNumPRG;
        mNumCHR = inNumCHR;
        
        mShiftIndex = 0;
        mTempReg    = 0;
        mControl    = 0x0C;
        mCHRReg0    = 0;
        mCHRReg1    = 0;
        mPRGReg     = 0;
    }
    
    virtual UInt32  Map(UInt16 inAddr) const
    {
        UInt32 base = inAddr - 0x8000;
        UInt32 ea = inAddr;
        UInt8  bank_idx = mPRGReg&0x0F;

        switch((mControl >> 2 ) & 0x03)
        {
            case 0:
            case 1:
                ea = base + (mPRGReg&0x0E) * 0x4000;
                break;
                
            case 2:
                if (inAddr < 0xC000)
                    bank_idx = 1;
                else
                    bank_idx = bank_idx;

                ea = base + (bank_idx-1) * 0x4000;
                break;
                
            case 3:
                if (inAddr >= 0xC000)
                    bank_idx = mNumPRG-2;
                else
                    bank_idx = bank_idx;
                
                ea = base + bank_idx * 0x4000;
                break;
                
                ea = ea;
        }
        
        return ea;
    }
    
    virtual void Store(UInt16 inAddr, UInt8 inData)
    {
        // if not reset
        if (/*true || */(inData & 0x80) == 0)
        {
            mTempReg |= (inData & 0x01) << mShiftIndex++;
            
            if (mShiftIndex == 5)
            {
                int idx = (inAddr >> 13) & 0x3;
                mRegisters[idx] = mTempReg;
                
                mShiftIndex = 0;
                mTempReg = 0;
            }
        }
        else
        {
            mShiftIndex = 0;
            mTempReg = 0;
            mControl |= 0x0C;
        }
    }
    
    virtual UInt8   GetPRGBankIdx() const
    {
        return mPRGReg & 0x0F;
    }

    UInt8   mNumPRG;
    UInt8   mNumCHR;
    
    UInt8   mShiftIndex;
    UInt8   mTempReg;

    union {
        struct {
            UInt8   mControl;
            UInt8   mCHRReg0;       // CHR select 0
            UInt8   mCHRReg1;       // CHR select 1
            UInt8   mPRGReg;        // PRG select / WRAM enable
        };
        UInt8  mRegisters[4];
    };
    
};

#endif
