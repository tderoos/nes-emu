// nes-emu IO module
#pragma once

#include "common/types.h"

// Forward declarations
class SaveState;
class LoadState;
class Ram;
class Rom;
class PPU2C07;
class APU;

class IO
{
public:
	IO(Ram* inRam, Rom* inRom, PPU2C07* inPPU, APU* inAPU);

	void SetButtonState(const uint8 inState) { mButtonState = inState; }

	void Tick();
	void Load(uint16 inAddr, uint8* outValue);
	void Store(uint16 inAddr, uint8 inValue);

	bool IsDMA() const { return mDMAState != DMAState::Inactive; }
	bool IRQ() const;
	bool NMI() const;

	// State saving
	void ReadState(const LoadState& ioState);
	void WriteState(SaveState& ioState) const;

private:
	Ram*	 mRam;
	Rom*	 mRom;
	PPU2C07* mPPU;
	APU*	 mAPU;

	uint8	 mButtonState = 0;
	uint8	 mButtonReadMask = 0;

	enum class DMAState
	{
		Read,
		Write,
		Inactive
	};
	DMAState mDMAState = DMAState::Inactive;
	uint16	 mDMASrc = 0;
	uint8	 mDMAValue = 0;
};
