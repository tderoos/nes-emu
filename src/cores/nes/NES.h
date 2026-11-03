// nes-emu
#pragma once

#include "APU.h"
#include "common/types.h"
#include "CPU6502.h"
#include "IO.h"
#include "PPU2C07.h"
#include "Ram.h"
#include "Rom.h"

// Forward declarations
class CartridgeData;
class SaveState;
class LoadState;

class NES
{
public:
	explicit NES(const CartridgeData& inCartridge);

	void RunToVBlank(uint8 inButtonState, uint32* ioFrameBuffer, uint8* ioAudioBuffer, uint32* ioSampleCount);

	// State saving. The snapshot starts with sStateVersion so the core can
	// reject or migrate snapshots written by an incompatible core version.
	static constexpr uint32 sStateVersion = 1;

	bool ReadState(const LoadState& ioState);
	void WriteState(SaveState& ioState) const;

private:
	Rom		  mRom;
	Ram		  mRam;
	PPU2C07	  mPPU;
	APU		  mAPU;
	IO		  mIO;
	CPU6502	  mCPU;
};
