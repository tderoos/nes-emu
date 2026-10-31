// nes-emu Rom module
#pragma once

#include <string>

#include "common/types.h"
#include "Mapper.h"
#include "SaveState.h"

// Forward declarations
class CartridgeData;

class Mapper;

class Rom
{
public:
	// Construct and load
	Rom(const CartridgeData& inCartridge);
	void SetVRam(uint8* mVRam);

	// Rom info
	inline EVRamLayout GetVRamMirroring() const { return mVRamMirrorMapped; }

	// Load
	inline void Load(uint16 inAddr, uint8* outValue) const
	{
		if (inAddr >= 0x6000 && inAddr < 0x8000)
		{
			*outValue = mPRGRam[inAddr - 0x6000];
		}
		else
			*outValue = mPRGData[inAddr & 0x7FFF];
	};

	// Store
	void Store(uint16 inAddr, uint8 inValue);

	// Game saving
	void SaveGameState() const;

	// State saving
	void ReadState(const SaveState& ioState);
	void WriteState(SaveState& ioState) const;

private:
	const CartridgeData& mCartridge;

	std::string mSavename;

	bool		mSRam = false;
	bool		mSRamBattery = false;
	EVRamLayout mVRamMirrorRom;
	EVRamLayout mVRamMirrorMapped;

	const uint8*		mRomData = nullptr;
	Mapper*				mMapper = nullptr;

	// Remapped data - no remapping during normal runtime
	// Kept up to date by the Mapper.
	uint8  mPRGData[0x8000] = {};
	uint8* mCHRData = nullptr;
	uint8  mPRGRam[0x2000] = {};
};
