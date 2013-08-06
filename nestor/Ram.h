//
// Created by tderoos on 3/3/13.
//
// To change the template use AppCode | Preferences | File Templates.
//



#ifndef __Ram_H_
#define __Ram_H_

#include "types.h"


class Ram
{
public:
    Ram(uint16 inSize);
    ~Ram();

    // Access
    inline void Load(uint16 inAddr, uint8* outValue) const      { *outValue = mData[inAddr & 0x07FF]; }
    inline void Store(uint16 inAddr, uint8 inValue)             { mData[inAddr & 0x07FF] = inValue;   }

private:
    uint8* mData;
};

#endif //__Ram_H_
