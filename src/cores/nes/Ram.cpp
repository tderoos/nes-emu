// nes-emu Ram module
#include "Ram.h"

#include <cstring>

Ram::Ram(uint16 inSize) : mSize(inSize), mData(std::make_unique<uint8[]>(inSize))
{
	// Power-on state: open bus reads as 0xFF
	memset(mData.get(), 0xFF, mSize);
}

void Ram::ReadState(const SaveState& ioState)
{
	uint16 size;
	ioState.Read(size);

	// The RAM size is fixed for the lifetime of the machine. A savestate
	// with a different size belongs to a different machine: skip its data
	// rather than resizing (and then desyncing every component after us).
	if (size != mSize)
	{
		ioState.Skip(size);
		return;
	}

	ioState.ReadRaw(mData.get(), mSize);
}

void Ram::WriteState(SaveState& ioState) const
{
	ioState.Write(mSize);
	ioState.WriteRaw(mData.get(), mSize);
}
