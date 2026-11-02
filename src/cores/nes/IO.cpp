// nes-emu IO Module
#include "IO.h"
#include "APU.h"
#include "PPU2C07.h"
#include "Ram.h"
#include "Rom.h"

void BREAK();

IO::IO(Ram* inRam, Rom* inRom, PPU2C07* inPPU, APU* inAPU)
	: mRam(inRam), mRom(inRom), mPPU(inPPU), mAPU(inAPU)
{
}

bool IO::IRQ() const
{
	return mAPU->GetInterrupt();
}

bool IO::NMI() const
{
	return mPPU->GetNMI();
}

void IO::Tick()
{
	switch (mDMAState)
	{
	case DMAState::Read:
		Load(mDMASrc++, &mDMAValue);
		mDMAState = DMAState::Write;
		break;

	case DMAState::Write:
		mPPU->Store(0x2004, mDMAValue);
		mDMAState = (mDMASrc & 0xFF) ? DMAState::Read : DMAState::Inactive;
		break;

	case DMAState::Inactive:
		break;
	}
}

void IO::Load(const uint16 inAddr, uint8* outValue)
{
	if (inAddr < 0x2000)
		mRam->Load(inAddr, outValue);

	else if (inAddr < 0x4000)
		mPPU->Load(inAddr, outValue);

	// Controller
	else if (inAddr == 0x4016)
	{
		// Set b5 (connected)
		*outValue = 0x40;
		if (mButtonState & mButtonReadMask)
			*outValue += 1;
		mButtonReadMask <<= 1;
	}
	else if (inAddr == 0x4017)
		*outValue = 0x40;

	else if (inAddr < 0x4018)
	{
		// APU
		mAPU->Load(inAddr, outValue);
	}
	else if (inAddr < 0x6000)
	{ // EROM
	}
	else
		mRom->Load(inAddr, outValue);
}

void IO::Store(const uint16 inAddr, const uint8 inValue)
{
	if (inAddr < 0x2000)
		mRam->Store(inAddr, inValue);

	else if (inAddr < 0x4000)
		mPPU->Store(inAddr, inValue);

	// DMA
	else if (inAddr == 0x4014)
	{
		mDMASrc = inValue << 8;
		mDMAState = DMAState::Read;
	}

	// Controller strobe
	else if (inAddr == 0x4016)
	{
		if ((inValue & 1) == 0)
			mButtonReadMask = 1;
	}

	else if (inAddr < 0x4018)
	{ // APU
		mAPU->Store(inAddr, inValue);
	}

	else if (inAddr < 0x6000)
	{ // EROM
	}
	else
		mRom->Store(inAddr, inValue);
}

void IO::ReadState(const SaveState& ioState)
{
	ioState.Read(mButtonState);
	ioState.Read(mButtonReadMask);
	ioState.Read(mDMAState);
	ioState.Read(mDMASrc);
	ioState.Read(mDMAValue);
}

void IO::WriteState(SaveState& ioState) const
{
	ioState.Write(mButtonState);
	ioState.Write(mButtonReadMask);
	ioState.Write(mDMAState);
	ioState.Write(mDMASrc);
	ioState.Write(mDMAValue);
}
