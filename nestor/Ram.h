//
// Created by tderoos on 3/3/13.
//
// To change the template use AppCode | Preferences | File Templates.
//



#ifndef __Ram_H_
#define __Ram_H_

#include <stdint.h>


class Ram
{
public:
    Ram(uint16_t inSize);

    // Access
    inline void Load(uint16_t inAddr, uint8_t* outValue) const      { *outValue = mData[inAddr & 0x07FF]; }
    inline void Store(uint16_t inAddr, uint8_t inValue)             { mData[inAddr & 0x07FF] = inValue;   }

private:
    uint8_t* mData;
};

#endif //__Ram_H_
