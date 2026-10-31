// nes-emu Rom module
#include "Rom.h"
#include "cores/CartridgeData.h"
#include "Mapper.h"



Rom::Rom(const CartridgeData& inCartridge) : mCartridge(inCartridge)
{
	const uint8* cartridge_data = inCartridge.GetData();

	uint8 num_prg = cartridge_data[4];
	uint8 num_chr = cartridge_data[5];

	uint8 mapper_id = cartridge_data[6] >> 4;
	mapper_id = mapper_id | (cartridge_data[7] & 0xF0);

	mSRam = (cartridge_data[10] & 0x10) != 0;
	mSRamBattery = (cartridge_data[6] & 0x02) != 0;

	switch (cartridge_data[6] & 9)
	{
	case 0:
		mVRamMirrorRom = HORIZONTAL;
		break;
	case 1:
		mVRamMirrorRom = VERTICAL;
		break;
	case 8:
		mVRamMirrorRom = FOUR_SCREEN;
		break;
	}
	mVRamMirrorMapped = mVRamMirrorRom;

	mRomData = cartridge_data + 16; // sizeof(header);

	printf("Loaded %s (PRG:%d CHR:%d MAP:%d VRM:%d SR:%d SRB:%d)\n", mCartridge.GetFilename().c_str(), num_prg, num_chr, mapper_id,
		   (int)GetVRamMirroring(), (int)mSRam, (int)mSRamBattery);

	mMapper = Mapper::sCreate(mapper_id, num_prg, num_chr);

	// Load savegame
	if (mSRamBattery)
	{
		mSavename = mCartridge.GetFilename();
		size_t idx = mSavename.find(".nes");
		mSavename.replace(idx, idx + 4, ".sav");

		FILE* savegame = fopen(mSavename.c_str(), "rb");
		if (savegame != nullptr)
		{
			fread(mPRGRam, 1, 0x2000, savegame);
			fclose(savegame);
		}
		else
			memset(mPRGRam, 0, 0x2000);
	}
}



// Set VRam - allows mapper to update CHR page directly into VRam
void Rom::SetVRam(uint8* mVRam)
{
	mCHRData = mVRam;
	mMapper->UpdateMapping(mRomData, mPRGData, mCHRData, mVRamMirrorRom, &mVRamMirrorMapped);
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
			mMapper->UpdateMapping(mRomData, mPRGData, mCHRData, mVRamMirrorRom, &mVRamMirrorMapped);
	}
}



// If rom has SRam - save state on exit
void Rom::SaveGameState() const
{
	if (mSRamBattery)
	{
		FILE* savegame = fopen(mSavename.c_str(), "w");
		if (savegame != nullptr)
		{
			fwrite(mPRGRam, 1, 0x2000, savegame);
			fclose(savegame);
		}
	}
}



void Rom::ReadState(const SaveState& ioState)
{
	if (mSRam)
		ioState.ReadRaw(mPRGRam, 0x2000);

	mMapper->ReadState(ioState);

	// Update the mapping after restoring the state
	mMapper->UpdateMapping(mRomData, mPRGData, mCHRData, mVRamMirrorRom, &mVRamMirrorMapped);
}


void Rom::WriteState(SaveState& ioState) const
{
	if (mSRam)
		ioState.WriteRaw(mPRGRam, 0x2000);

	mMapper->WriteState(ioState);
}
