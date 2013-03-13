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
    Rom(const char* inFilename);

    // Properties
    char GetNumPRGBanks() const                 { return mNumPRG; }
    char GetNumCHRBanks() const                 { return mNumCHR; }
    char GetMapperID() const;

    // Access
    void Load(UInt16 inAddr, UInt8* outValue) const;
    void Store(UInt16 inAddr, UInt8 inValue);
    
    const UInt8*  GetCHRData(int inBank) const;

private:
    UInt8 mNumPRG;
    UInt8 mNumCHR;
    UInt8 mMapperID;
    
    bool  mSRam;
    bool  mSRamBattery;
    
    UInt8*  mData;
    Mapper* mMapper;
};

#endif //__Rom_H_
