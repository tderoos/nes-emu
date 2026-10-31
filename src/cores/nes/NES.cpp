// nes-emu
#include "NES.h"

NES::NES(const CartridgeData& inCartridge) : mRom(inCartridge), mRam(0x8000), mPPU(&mRom), mIO(&mRam, &mRom, &mPPU, &mAPU), mCPU(&mIO) {}

void NES::onExit()
{
	mRom.SaveGameState();
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

// 7458.333333333333333

// State saving
void NES::ReadState(const SaveState& ioState)
{
	if (ioState.mData.empty())
		return;

	ioState.mReadOffset = 0;
	mRom.ReadState(ioState);
	mRam.ReadState(ioState);
	mPPU.ReadState(ioState);
	mAPU.ReadState(ioState);
	mIO.ReadState(ioState);
	mCPU.ReadState(ioState);
}

void NES::WriteState(SaveState& ioState) const
{
	ioState.mReadOffset = 0;
	ioState.mData.clear();

	mRom.WriteState(ioState);
	mRam.WriteState(ioState);
	mPPU.WriteState(ioState);
	mAPU.WriteState(ioState);
	mIO.WriteState(ioState);
	mCPU.WriteState(ioState);
}
