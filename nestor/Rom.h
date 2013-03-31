//
// Created by tderoos on 3/3/13.
//
// To change the template use AppCode | Preferences | File Templates.
//



#ifndef __Rom_H_
#define __Rom_H_

#include "types.h"
#include "Mapper.h"
#include <string>

class Mapper;


class Rom {
public:
    // Construct and load
                        Rom(const char* inFilename);
    void                SetVRam(uint8* mVRam);
    
    // Rom info
    inline EVRamLayout  GetVRamMirroring() const                        { return mVRamMirrorMapped; }

    // Load
    inline void         Load(uint16 inAddr, uint8* outValue) const
    {
        if (inAddr >= 0x6000 && inAddr < 0x8000)
        {
            *outValue = mPRGRam[inAddr - 0x6000];
        }
        else
            *outValue = mPRGData[inAddr & 0x7FFF];
    };

    // Store
    void                Store(uint16 inAddr, uint8 inValue);
    
    // Game saving
    void                SaveGameState();
    

private:
    std::string mFilename;
    std::string mSavename;
    
    bool        mSRam;
    bool        mSRamBattery;
    EVRamLayout mVRamMirrorRom;
    EVRamLayout mVRamMirrorMapped;
    
    uint8*      mData;
    Mapper*     mMapper;
    
    // Remapped data - no remapping during normal runtime
    // Kept up to date by the Mapper.
    uint8   mPRGData[0x8000];
    uint8*  mCHRData;
    uint8   mPRGRam[0x2000];
};

#endif //__Rom_H_
