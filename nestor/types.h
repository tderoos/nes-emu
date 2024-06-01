// nes-emu Types
#pragma once

#include <stdint.h>
#include <vector>

typedef int8_t      int8;
typedef int16_t     int16;
typedef int32_t     int32;
typedef int64_t     int64;

typedef uint8_t     uint8;
typedef uint16_t    uint16;
typedef uint32_t    uint32;
typedef uint64_t    uint64;

struct SaveState
{
	template <typename taType>
	void	Read(taType& inValue) const
	{
		ReadRaw((uint8*)&inValue, sizeof(taType));
	}

	void ReadRaw(uint8* outData, int inSize) const
	{
		memcpy(outData, &mData[mReadOffset], inSize);
		mReadOffset += inSize;
	}

	template <typename taType>
	void	Write(const taType& inValue)
	{
		WriteRaw((const uint8*)&inValue, sizeof(taType));
	}

	void	WriteRaw(const uint8* inData, int inSize)
	{
		mData.insert(mData.end(), inData, inData + inSize);
	}

	std::vector<uint8>	mData;
	mutable int			mReadOffset = 0;
};