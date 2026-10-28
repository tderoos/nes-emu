#pragma once

#include <algorithm>
#include <cstring>
#include <stdint.h>

#include "Mutex.h"

/**
 * @brief Fixed-size thread-safe ring buffer
 *
 * @tparam taType  Element type
 * @tparam taSize  Capacity in elements
 */
template <typename taType, size_t taSize>
class RingBuffer
{
public:
	static constexpr size_t sMaxSize = taSize;

	int	Push(const taType* inData, uint32_t inCount)
	{
		AutoLockMutex lock(mMutex);

		const uint32_t count = std::min<uint32_t>(inCount, taSize - mSize);
		const uint32_t size1 = std::min<uint32_t>(count, taSize - mWriteIndex);
		const uint32_t size2 = count - size1;

		memcpy(&mData[mWriteIndex], inData, size1 * sizeof(taType));
		mWriteIndex += size1;
		if (size2) {
			memcpy(&mData[0], inData + size1, size2 * sizeof(taType));
			mWriteIndex = size2;
		}

		mSize += count;
		return count;
	}

	int	Pop(taType* outData, uint32_t inCount)
	{
		int remaining_size = 0;
		return Pop(outData, inCount, &remaining_size);
	}

	/**
	 * @brief Pop up to inCount elements; outRemainingSize reports how many
	 * elements are left in the buffer after this pop (atomically).
	 */
	int	Pop(taType* outData, uint32_t inCount, int* outRemainingSize)
	{
		AutoLockMutex lock(mMutex);

		const uint32_t count = std::min<uint32_t>(inCount, mSize);
		const uint32_t size1 = std::min<uint32_t>(count, taSize - mReadIndex);
		const uint32_t size2 = count - size1;

		memcpy(outData, &mData[mReadIndex], size1 * sizeof(taType));
		mReadIndex += size1;
		if (size2) {
			memcpy(outData + size1, &mData[0], size2 * sizeof(taType));
			mReadIndex = size2;
		}

		mSize -= count;
		if (outRemainingSize)
			*outRemainingSize = mSize;
		return count;
	}

	void Clear()
	{
		AutoLockMutex lock(mMutex);
		mSize = mReadIndex = mWriteIndex = 0;
	}

	int		mSize = 0;
	int		mReadIndex = 0;
	int		mWriteIndex = 0;
	taType	mData[taSize] = {0};

private:
	Mutex	mMutex;
};
