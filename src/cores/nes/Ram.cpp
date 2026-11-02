// nes-emu Ram module
#include "Ram.h"

#include <cstring>

Ram::Ram(uint16 inSize) : mSize(inSize), mData(inSize, 0xFF) {}

void Ram::ReadState(const SaveState& ioState)
{
	uint16 size;
	ioState.Read(size);
	if (size != mSize)
	{
		mData.resize(size);
		mSize = size;
	}

	ioState.ReadRaw(mData.data(), mSize);
}

void Ram::WriteState(SaveState& ioState) const
{
	ioState.Write(mSize);
	ioState.WriteRaw(mData.data(), mSize);
}
