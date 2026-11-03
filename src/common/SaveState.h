#pragma once

#include <cstring>
#include <vector>

#include "common/types.h"

/**
 *
 */
class SaveState
{
public:
	void Clear()
	{
		mData.clear();
		mReadOffset = 0;
	}
	void Reset() const { mReadOffset = 0; }
	bool IsEmpty() const { return mData.empty(); }

	template <typename taType>
	void Read(taType& outValue) const
	{
		memcpy(&outValue, &mData[mReadOffset], sizeof(taType));
		mReadOffset += sizeof(taType);
	}

	template <typename taType>
	void Write(const taType& inValue)
	{
		const uint8* data = reinterpret_cast<const uint8*>(&inValue);
		mData.insert(mData.end(), data, data+sizeof(taType));
	}

private:
	std::vector<uint8> mData;
	mutable int		   mReadOffset = 0;
};