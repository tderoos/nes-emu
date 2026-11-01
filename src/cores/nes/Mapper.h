// nes-emu Mapper interface
#pragma once

#include "common/types.h"
#include "SaveState.h"

// Rom info
enum class EVRamLayout
{
	Vertical,
	Horizontal,
	FourScreen,
	SingleLower,
	SingleUpper
};

class Mapper
{
public:
	virtual ~Mapper() = default;

	// Factory
	static Mapper* sCreate(uint8 inType, uint8 inNumPRG, uint8 inNumCHR);

	bool		 IsDirty() const { return mDirty; }

	virtual void Store(uint16 inAddr, uint8 inData) = 0;
	virtual void UpdateMapping(const uint8* inData, uint8* ioPRG, uint8* ioCHR, EVRamLayout inRomLayout,
							   EVRamLayout* outMappedLayout) const = 0;

	// State saving
	virtual void ReadState(const SaveState& ioState) {}
	virtual void WriteState(SaveState& ioState) const {}

protected:
	Mapper(uint8 inNumPRG, uint8 inNumCHR) : mNumPRG(inNumPRG), mNumCHR(inNumCHR), mDirty(true) {}

	uint8		 mNumPRG;
	uint8		 mNumCHR;
	mutable bool mDirty;
};
