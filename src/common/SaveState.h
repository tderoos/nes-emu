#pragma once

#include "common/types.h"
#include <cstring>
#include <span>
#include <vector>

using StateData = std::vector<uint8>;
using StateDataSpan = std::span<const uint8>;

/**
 * LoadState interface
 */
class [[nodiscard]] LoadState
{
public:
	explicit LoadState(const StateDataSpan inState) : mState(inState) {}

	bool IsEmpty() const { return mState.empty(); }

	template <typename taType>
	bool Read(taType& outValue) const
	{
		// Bounds check
		if (sizeof(taType) > mState.size() - mOffset)
			return false;

		memcpy((void*) &outValue, &mState[mOffset], sizeof(taType));
		mOffset += sizeof(taType);
		return true;
	}

private:
	StateDataSpan mState;
	mutable int	  mOffset = 0;
};

/**
 * SaveState interface
 */
class [[nodiscard]] SaveState
{
public:
	explicit SaveState(StateData& inState) : mState(inState) {}

	template <typename taType>
	void Write(const taType& inValue)
	{
		const uint8* data = reinterpret_cast<const uint8*>(&inValue);
		mState.insert(mState.end(), data, data + sizeof(taType));
	}

private:
	StateData& mState;
};
