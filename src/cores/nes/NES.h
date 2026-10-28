// nes-emu
#pragma once

#include "APU.h"
#include "common/types.h"
#include "CPU6502.h"
#include "IO.h"
#include "PPU2C07.h"
#include "Ram.h"
#include "Rom.h"
#include "SaveState.h"

class NES
{
public:
                    NES(const char* inRom);
    void            onExit();

    void            RunToVBlank(uint8 inButtonState, uint32* ioFrameBuffer, uint8* ioAudioBuffer, uint32* ioSampleCount);

    // State saving
    void            ReadState(const SaveState& ioState);
    void            WriteState(SaveState& ioState) const;
 
private :
    uint32          mFrameBuffer[256*256];
    
    Rom             mRom;
    Ram             mRam;
    PPU2C07         mPPU;
    APU             mAPU;
    IO              mIO;
    CPU6502         mCPU;

    SaveState       mSaveState;
};
