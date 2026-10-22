// nes-emu Ram module
#include "Ram.h"
#include <stdlib.h>
#include <memory.h>


Ram::Ram(uint16 inSize)
{
    mSize = inSize;
    mData = (uint8 *) malloc(inSize);
    memset(mData, 0xFFFFFFFF, inSize);
}



Ram::~Ram()
{
    free(mData);
}



void Ram::ReadState(const SaveState& ioState)
{
    uint16 size;
    ioState.Read(size);
    if (size != mSize)
    {
        mData = (uint8*) realloc(mData, size);
        mSize = size;
    }

    ioState.ReadRaw(mData, mSize);
}



void Ram::WriteState(SaveState& ioState) const
{
    ioState.Write(mSize);
    ioState.WriteRaw(mData, mSize);
}
