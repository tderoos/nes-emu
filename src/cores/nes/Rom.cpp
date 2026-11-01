// nes-emu Rom module
#include "Rom.h"

#include <algorithm>
#include <cstring>
#include "cartridge/CartridgeData.h"
#include "Mapper.h"

Rom::Rom(const CartridgeData& inCartridge) : mCartridge(inCartridge)
{
	mHeader = reinterpret_cast<const RomHeader*>(inCartridge.GetData());

	// Determine name-table mapping
	if (mHeader->mAlternativeNameTable == 0)
		mVRamMirrorRom = mHeader->mNametableArrangement == 0 ? EVRamLayout::Horizontal : EVRamLayout::Vertical;
	else
		mVRamMirrorRom = EVRamLayout::FourScreen;
	mVRamMirrorMapped = mVRamMirrorRom;

	const uint8 mapper_id = mHeader->mMapperIDLo | mHeader->mMapperIDHi << 4;
	mMapper = Mapper::sCreate(mapper_id, mHeader->mNumPRG, mHeader->mNumCHR);

	printf("Loaded %s (PRG:%d CHR:%d MAP:%d VRM:%d SRB:%d)\n", mCartridge.GetFilename().c_str(), mHeader->mNumPRG, mHeader->mNumCHR, mapper_id,
		   (int)GetVRamMirroring(), (int)mHeader->mSRamBattery);

	// Load savegame
	memset(mPRGRam, 0, 0x2000);

	if (mHeader->mSRamBattery)
	{
		std::span<uint8> persistent_state;
		if (mCartridge.GetPersistentState(persistent_state))
			memcpy(mPRGRam, persistent_state.data(), std::min(persistent_state.size(), (size_t)0x2000));
	}
}



Rom::~Rom()
{
	if (mHeader->mSRamBattery)
		mCartridge.SetPersistentState(mPRGRam);

	if (mMapper)
		delete mMapper;
}



// Set VRam - allows mapper to update CHR page directly into VRam
void Rom::SetVRam(uint8* mVRam)
{
	mCHRData = mVRam;
	mMapper->UpdateMapping(mHeader->mData, mPRGData, mCHRData, mVRamMirrorRom, &mVRamMirrorMapped);
}



// Store are passed on to the mapper
void Rom::Store(uint16 inAddr, uint8 inValue)
{
	if (inAddr >= 0x6000 && inAddr < 0x8000)
	{
		mPRGRam[inAddr - 0x6000] = inValue;
	}
	else
	{
		mMapper->Store(inAddr, inValue);
		if (mMapper->IsDirty())
			mMapper->UpdateMapping(mHeader->mData, mPRGData, mCHRData, mVRamMirrorRom, &mVRamMirrorMapped);
	}
}




void Rom::ReadState(const SaveState& ioState)
{
	if (mHeader->mSRamBattery)
		ioState.ReadRaw(mPRGRam, 0x2000);

	mMapper->ReadState(ioState);

	// Update the mapping after restoring the state
	mMapper->UpdateMapping(mHeader->mData, mPRGData, mCHRData, mVRamMirrorRom, &mVRamMirrorMapped);
}


void Rom::WriteState(SaveState& ioState) const
{
	if (mHeader->mSRamBattery)
		ioState.WriteRaw(mPRGRam, 0x2000);

	mMapper->WriteState(ioState);
}
