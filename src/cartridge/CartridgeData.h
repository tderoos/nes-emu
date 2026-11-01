#pragma once
#include "common/types.h"
#include <string>
#include <vector>
#include <span>

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
	// Loading data
	bool Load(const char* inFilename);
	void Clear();

	// Properties
	const std::string& GetFilename() const { return mFilename; }
	CartridgeType GetType() const	{ return mType; }

	// Data access
	uint32 GetSize() const			{ return mData.size(); }
	const uint8* GetData() const	{ return mData.data(); }

	// Persistent state
	void SetPersistentState(std::span<uint8> inState) const;
	bool GetPersistentState(std::span<uint8>& outState) const;
	void SavePersistentState() const;

private:
	std::string mFilename;
	CartridgeType mType = CartridgeType::Unknown;

	std::vector<uint8> mData;
	mutable std::vector<uint8> mPersistentState;
};
