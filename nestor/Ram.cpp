//
// Created by tderoos on 3/3/13.
//
// To change the template use AppCode | Preferences | File Templates.
//


#include "Ram.h"


Ram::Ram(uint16_t inSize)
{
    mData = (uint8_t *) malloc(inSize);
    memset(mData, 0xFFFFFFFF, inSize);
}



// Access
void Ram::Load(uint16_t inAddr, uint8_t* outValue) const
{
    *outValue = mData[inAddr & 0x07FF];
}



void Ram::Store(uint16_t inAddr, uint8_t inValue)
{
    mData[inAddr & 0x07FF] = inValue;
}
