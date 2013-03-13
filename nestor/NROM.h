//
//  NROM.h
//  nestor
//
//  Created by Tommy de Roos on 3/13/13.
//
//

#ifndef nestor_NROM_h
#define nestor_NROM_h

#include "Mapper.h"
#include <string.h>

class NROM : public Mapper
{
public:
    NROM(UInt8 inNumPRG, UInt8 inNumCHR) : Mapper(inNumPRG, inNumCHR)
    {
    }

    
    virtual void Store(UInt16 inAddr, UInt8 inData)
    {
        // Ignore - no mapping
    }
    
    
    virtual void UpdateMapping(const UInt8* inData, UInt8* ioPRG, UInt8* ioCHR) const
    {
        if (mNumPRG == 1)
        {
            // Mirror single PRG block in both hi and lo slot
            memcpy(ioPRG,         inData, 0x4000);
            memcpy(ioPRG+ 0x4000, inData, 0x4000);
        }
        else
        {
            memcpy(ioPRG, inData, 0x8000);
        }
        
        // Chr data
        if (mNumCHR != 0)
            memcpy(ioCHR, inData + mNumPRG * 0x4000, 0x2000);
        
        mDirty = false;
    }
};

#endif
