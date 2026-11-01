// nes-emu Rom module
#pragma once

#include <string>

#include "common/types.h"
#include "Mapper.h"
#include "SaveState.h"

// Forward declarations
class CartridgeData;

class Mapper;

/**
 * RomHeader - implements the basic iNes rom header format.
 * For extended features Nes2 header support needs to be implemented.
 */
struct RomHeader
{
	char	mID[4];
	uint8	mNumPRG;
	uint8	mNumCHR;

	// FLags 6
	uint8	mNametableArrangement : 1;
	bool	mSRamBattery : 1;
	bool	mTrainer : 1;
	bool	mAlternativeNameTable : 1;
	uint8	mMapperIDLo: 4;

	// Flags 7
	uint8	mVSUnitSystem : 1;
	uint8	mPlayChoice10 : 1;
	uint8	mIsNes2Header : 2;
	uint8	mMapperIDHi : 4;

	uint8	mPRGRamSize;

	// Flags9
	bool	mTVSystem : 1;
	uint8	mPadding9 : 7;

	uint8	mPadding[6];	///< Padding
	uint8	mData[];		///< Data starts here
};

class Rom
{
public:
	// Construct and load
	explicit Rom(const CartridgeData& inCartridge);
	~Rom();

	void SetVRam(uint8* mVRam);

	// Rom info
	EVRamLayout GetVRamMirroring() const { return mVRamMirrorMapped; }

	// Load
	void Load(uint16 inAddr, uint8* outValue) const
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

	// State saving
	void ReadState(const SaveState& ioState);
	void WriteState(SaveState& ioState) const;

private:
	const CartridgeData& mCartridge;
	const RomHeader*	 mHeader;

	EVRamLayout			 mVRamMirrorRom;
	EVRamLayout			 mVRamMirrorMapped;
	Mapper*				 mMapper = nullptr;

	// Remapped data - no remapping during normal runtime
	// Kept up to date by the Mapper.
	uint8  mPRGData[0x8000] = {};
	uint8* mCHRData = nullptr;
	uint8  mPRGRam[0x2000] = {};
};
