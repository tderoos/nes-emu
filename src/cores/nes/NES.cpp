// NES Emulator Core
#include "NES.h"

NES::NES(std::span<const uint8> inRom)
	: mRom(inRom), mPPU(&mRom), mIO(&mRam, &mRom, &mPPU, &mAPU), mCPU(&mIO)
{
}

void NES::RunToVBlank(uint8 inButtonState, uint32* ioFrameBuffer, uint8* ioAudioBuffer, uint32* ioSampleCount)
{
	mIO.SetButtonState(inButtonState);
	mPPU.SetFrameBuffer(ioFrameBuffer);
	mAPU.SetAudioBuffer(ioAudioBuffer);

	do
	{
		if (!mIO.IsDMA())
			mCPU.Tick(mPPU.GetClock(), mPPU.GetScanline());
		mPPU.Tick();
		mAPU.Tick();
		mIO.Tick();
	} while (!mPPU.SwapBuffer());

	if (ioSampleCount)
		*ioSampleCount = mAPU.GetSampleCount();
}

void NES::LoadPersistentState(const LoadState& inState) const
{
	mRom.LoadPersistentState(inState);
}

void NES::StorePersistentState(SaveState& outState) const
{
	mRom.StorePersistentState(outState);
}

// 7458.333333333333333

// State saving
bool NES::ReadState(const LoadState& ioState)
{
	uint32 state_version = 0;
	if (!ioState.Read(state_version) || state_version != sStateVersion)
		return false;

	mRom.ReadState(ioState);
	mRam.ReadState(ioState);
	mPPU.ReadState(ioState);
	mAPU.ReadState(ioState);
	mIO.ReadState(ioState);
	mCPU.ReadState(ioState);
	return true;
}

void NES::WriteState(SaveState& ioState) const
{
	ioState.Write(sStateVersion);
	mRom.WriteState(ioState);
	mRam.WriteState(ioState);
	mPPU.WriteState(ioState);
	mAPU.WriteState(ioState);
	mIO.WriteState(ioState);
	mCPU.WriteState(ioState);
}
