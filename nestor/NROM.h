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
    NROM(uint8 inNumPRG, uint8 inNumCHR) : Mapper(inNumPRG, inNumCHR)
    {
    }

    
    virtual void Store(uint16 inAddr, uint8 inData)
    {
        // Ignore - no mapping
    }
    
    
    virtual void UpdateMapping(const uint8* inData, uint8* ioPRG, uint8* ioCHR, EVRamLayout inRomLayout, EVRamLayout* outMappedLayout) const
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
        
        // Follow rom VRam settings
        *outMappedLayout = inRomLayout;
        
        mDirty = false;
    }
};

#endif
