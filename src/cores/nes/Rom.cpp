// nes-emu Rom module
#include "Rom.h"

#include "Mapper.h"

#include <algorithm>
#include <cstdio>
#include <cstring>

Rom::Rom(std::span<const uint8> inRom)
{
	mHeader = reinterpret_cast<const RomHeader*>(inRom.data());

	// Determine name-table mapping
	if (mHeader->mAlternativeNameTable == 0)
		mVRamMirrorRom = mHeader->mNametableArrangement == 0 ? EVRamLayout::Horizontal : EVRamLayout::Vertical;
	else
		mVRamMirrorRom = EVRamLayout::FourScreen;
	mVRamMirrorMapped = mVRamMirrorRom;

	const uint8 mapper_id = mHeader->mMapperIDLo | mHeader->mMapperIDHi << 4;
	mMapper = Mapper::sCreate(mapper_id, mHeader->mNumPRG, mHeader->mNumCHR);

	printf("NES Rom loaded (PRG:%d CHR:%d MAP:%d VRM:%d SRB:%d)\n", mHeader->mNumPRG,
		   mHeader->mNumCHR, mapper_id, static_cast<int>(GetVRamMirroring()), mHeader->mSRamBattery ? 1 : 0);

	// Clear persistent state savegame
	memset(mPRGRam, 0, 0x2000);
}

Rom::~Rom()
{
	delete mMapper;
}

void Rom::LoadPersistentState(const LoadState& inState) const
{
	if (mHeader->mSRamBattery)
		inState.Read(mPRGRam);
}

void Rom::StorePersistentState(SaveState& outState) const
{
	if (mHeader->mSRamBattery)
		outState.Write(mPRGRam);
}

// Set VRam - allows mapper to update CHR page directly into VRam
void Rom::SetVRam(uint8* mVRam)
{
	mCHRData = mVRam;
	if (mMapper)
		mMapper->UpdateMapping(mHeader->mData, mPRGData, mCHRData, mVRamMirrorRom, &mVRamMirrorMapped);
}

// Store are passed on to the mapper
void Rom::Store(uint16 inAddr, uint8 inValue)
{
	if (inAddr >= 0x6000 && inAddr < 0x8000)
	{
		mPRGRam[inAddr - 0x6000] = inValue;
	}
	else if (mMapper)
	{
		mMapper->Store(inAddr, inValue);
		if (mMapper->IsDirty())
			mMapper->UpdateMapping(mHeader->mData, mPRGData, mCHRData, mVRamMirrorRom, &mVRamMirrorMapped);
	}
}

void Rom::ReadState(const LoadState& ioState)
{
	if (mHeader->mSRamBattery)
		ioState.Read(mPRGRam);

	if (mMapper)
	{
		mMapper->ReadState(ioState);

		// Update the mapping after restoring the state
		mMapper->UpdateMapping(mHeader->mData, mPRGData, mCHRData, mVRamMirrorRom, &mVRamMirrorMapped);
	}
}

void Rom::WriteState(SaveState& ioState) const
{
	if (mHeader->mSRamBattery)
		ioState.Write(mPRGRam);

	if (mMapper)
		mMapper->WriteState(ioState);
}
