#pragma once
#include "common/types.h"

#include <string>

enum class CartridgeType
{
	NES,
	Unknown,
};

/**
 *
 */
class CartridgeData
{
public:
	//
	bool Load(const char* inFilename);
	void Clear();

	// Properties
	const std::string& GetFilename() const { return mFilename; }
	CartridgeType GetType() const	{ return mType; }

	// Data access
	uint32 GetSize() const			{ return mSize; }
	const uint8* GetData() const	{ return mData; }


private:
	std::string mFilename;
	CartridgeType mType = CartridgeType::Unknown;
	uint32 mSize = 0;
	uint8* mData = nullptr;
};
