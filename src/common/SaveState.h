#pragma once

#include <cstring>
#include <span>
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

	// Raw blob access (e.g. to adopt a snapshot loaded from a .sav file)
	void					SetData(std::span<const uint8> inData)
	{
		mData.assign(inData.begin(), inData.end());
		mReadOffset = 0;
	}
	std::span<const uint8> GetData() const { return mData; }

	template <typename taType>
	bool Read(taType& outValue) const
	{
		// Bounds check
		if (sizeof(taType) > mData.size() - mReadOffset)
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