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

	size_t Remaining() const { return mData.size() > static_cast<size_t>(mReadOffset) ? mData.size() - mReadOffset : 0; }
	bool   CanRead(size_t inSize) const { return inSize <= Remaining(); }

	template <typename taType>
	bool Read(taType& outValue) const
	{
		if (!CanRead(sizeof(taType)))
			return false;
		memcpy(&outValue, &mData[mReadOffset], sizeof(taType));
		mReadOffset += sizeof(taType);
		return true;
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