//
// Created by tderoos on 3/3/13.
//
// To change the template use AppCode | Preferences | File Templates.
//


#include "Ram.h"
#include <stdlib.h>
#include <memory.h>


Ram::Ram(uint16 inSize)
{
    mData = (uint8 *) malloc(inSize);
    memset(mData, 0xFFFFFFFF, inSize);
}



Ram::~Ram()
{
    free(mData);
}
