// nes-emu Mapper interface
#include "Mapper.h"
#include <cassert>
#include "Mappers/MMC1.h"
#include "Mappers/NROM.h"

Mapper* Mapper::sCreate(uint8 inType, uint8 inNumPRG, uint8 inNumCHR)
{
	switch (inType)
	{
	case 0:
		return new NROM(inNumPRG, inNumCHR);
	case 1:
		return new MMC1(inNumPRG, inNumCHR);
	default:
		assert(false);
	}

	return nullptr;
}