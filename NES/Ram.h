// nes-emu Ram module
#pragma once

#include "types.h"


class Ram
{
public:
    Ram(uint16 inSize);
    ~Ram();

    // Access
    inline void Load(uint16 inAddr, uint8* outValue) const      { *outValue = mData[inAddr & 0x07FF]; }
    inline void Store(uint16 inAddr, uint8 inValue)             { mData[inAddr & 0x07FF] = inValue;   }

    // State saving
    void		ReadState(const SaveState& ioState);
    void		WriteState(SaveState& ioState) const;

private:
    uint16 mSize;
    uint8* mData;
};
