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
class SaveState;
class LoadState;

class NES
{
public:
	explicit NES(std::span<const uint8> inRom);

	void RunToVBlank(uint8 inButtonState, uint32* ioFrameBuffer, uint8* ioAudioBuffer, uint32* ioSampleCount);

	// State saving. The snapshot starts with sStateVersion so the core can
	// reject or migrate snapshots written by an incompatible core version.
	static constexpr uint32 sStateVersion = 1;

	// Load/store system state
	bool ReadState(const LoadState& ioState);
	void WriteState(SaveState& ioState) const;

	// Load/store persistent (battery backed) state
	void LoadPersistentState(const LoadState& inState) const;
	void StorePersistentState(SaveState& outState) const;

private:
	Rom		  mRom;
	Ram		  mRam;
	PPU2C07	  mPPU;
	APU		  mAPU;
	IO		  mIO;
	CPU6502	  mCPU;
};
