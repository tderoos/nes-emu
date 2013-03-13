//
// Created by tderoos on 3/3/13.
//
// To change the template use AppCode | Preferences | File Templates.
//



#ifndef __Rom_H_
#define __Rom_H_

#include "types.h"

class Mapper;


class Rom {
public:
    // Construct and load
    Rom(const char* inFilename);

    // Load
    inline void Load(UInt16 inAddr, UInt8* outValue) const      { *outValue = mPRGData[inAddr & 0x7FFF]; };
    const UInt8*  GetCHRData() const                            { return mCHRData; }

    // Store
    void Store(UInt16 inAddr, UInt8 inValue);
    

private:
    bool  mSRam;
    bool  mSRamBattery;
    
    UInt8*  mData;
    Mapper* mMapper;
    
    // Remapped data - no remapping during normal runtime
    // Kept up to date by the Mapper. 
    UInt8   mPRGData[0x8000];
    UInt8   mCHRData[0x2000];
};

#endif //__Rom_H_
