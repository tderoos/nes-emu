// nes-emu 
#include "nestor.h"



nestor::nestor(const char* inRom) :
    mRom(inRom),
    mRam(0x8000),
    mPPU(&mRom),
    mIO(&mRam, &mRom, &mPPU, &mAPU),
    mCPU(&mIO)
{
}


void nestor::onExit()
{
    mRom.SaveGameState();
}


void nestor::RunToVBlank(char inButtonState, uint32* ioFrameBuffer, uint8* ioAudioBuffer, uint32* sampleCount)
{
    mIO.SetButtonState(inButtonState);
    mPPU.SetFrameBuffer(ioFrameBuffer);
    mAPU.SetAudioBuffer(ioAudioBuffer);
    
    int cycles = 0;
    
    do
    {
        if (!mIO.IsDMA())
            mCPU.Tick(mPPU.GetClock(), mPPU.GetScanline());
        mPPU.Tick();
        mAPU.Tick();
        mIO.Tick();

        cycles++;
    }
    while (!mPPU.SwapBuffer());
    if (sampleCount)
        *sampleCount = mAPU.GetSampleCount();
    cycles = cycles;
}

//7458.333333333333333


// State saving
void nestor::ReadState(const SaveState& ioState)
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


void nestor::WriteState(SaveState& ioState) const
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

