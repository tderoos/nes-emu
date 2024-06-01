// nes-emu Mapper interface
#pragma once

#include "types.h"

// Rom info
enum EVRamLayout
{
    VERTICAL,
    HORIZONTAL,
    FOUR_SCREEN,
    SINGLE_LOWER,
    SINGLE_UPPER
};

class Mapper
{
public:
    static Mapper* sCreate(uint8 inType, uint8 inNumPRG, uint8 inNumCHR);

    Mapper(uint8 inNumPRG, uint8 inNumCHR) :
        mNumPRG(inNumPRG),
        mNumCHR(inNumCHR),
        mDirty(true)                        { }

    inline bool     IsDirty() const         { return mDirty; }
    
    virtual void    Store(uint16 inAddr, uint8 inData) = 0;
    virtual void    UpdateMapping(const uint8* inData, uint8* ioPRG, uint8* ioCHR, EVRamLayout inRomLayout, EVRamLayout* outMappedLayout) const = 0;

    // State saving
    virtual void    ReadState(const SaveState& ioState) {}
    virtual void    WriteState(SaveState& ioState) const {}
    
protected:
    uint8           mNumPRG;
    uint8           mNumCHR;
    mutable bool    mDirty;
};
