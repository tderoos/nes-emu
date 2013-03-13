//
// Created by tderoos on 3/3/13.
//
// To change the template use AppCode | Preferences | File Templates.
//


#include "Ram.h"
#include <stdlib.h>
#include <memory.h>


Ram::Ram(uint16_t inSize)
{
    mData = (uint8_t *) malloc(inSize);
    memset(mData, 0xFFFFFFFF, inSize);
}
