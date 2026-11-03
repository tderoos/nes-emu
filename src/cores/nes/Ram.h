// nes-emu Ram module
#pragma once

#include "common/types.h"
#include "common/SaveState.h"

/**
 * NES Ram module - 2KiB Memory module mapped to 0x0000 - 0x07FF
 */
class Ram
{
public:
	// Settings
	static constexpr int sSize = 0x0800;
	static constexpr int sMask = sSize - 1;

	// Access
	void Load(const uint16 inAddr, uint8* outValue) const { *outValue = mData[inAddr & sMask]; }
	void Store(const uint16 inAddr, const uint8 inValue) { mData[inAddr & sMask] = inValue; }

	// State saving
	void ReadState(const LoadState& ioState) { ioState.Read(mData); }
	void WriteState(SaveState& ioState) const { ioState.Write(mData); }

private:
	// Data
	uint8 mData[sSize] = {0xFF};
};
