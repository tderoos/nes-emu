#pragma once

#include <cstring>
#include <vector>

#include "common/types.h"

struct SaveState
{
	template <typename taType>
	void Read(taType& inValue) const
	{
		ReadRaw((uint8*)&inValue, sizeof(taType));
	}

	void ReadRaw(uint8* outData, int inSize) const
	{
		memcpy(outData, &mData[mReadOffset], inSize);
		mReadOffset += inSize;
	}

	void Skip(int inSize) const { mReadOffset += inSize; }

	template <typename taType>
	void Write(const taType& inValue)
	{
		WriteRaw((const uint8*)&inValue, sizeof(taType));
	}

	void WriteRaw(const uint8* inData, int inSize)
	{
		mData.insert(mData.end(), inData, inData + inSize);
	}

	std::vector<uint8> mData;
	mutable int		   mReadOffset = 0;
};