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

// Forward declarations
class CartridgeData;

class NES
{
public:
	NES(const CartridgeData& inCartridge);
	void onExit();

	void RunToVBlank(uint8 inButtonState, uint32* ioFrameBuffer, uint8* ioAudioBuffer, uint32* ioSampleCount);

	// State saving
	void ReadState(const SaveState& ioState);
	void WriteState(SaveState& ioState) const;

private:
	Rom		  mRom;
	Ram		  mRam;
	PPU2C07	  mPPU;
	APU		  mAPU;
	IO		  mIO;
	CPU6502	  mCPU;
	SaveState mSaveState;
};
