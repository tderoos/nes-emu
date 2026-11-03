#pragma once
#include "common/types.h"
#include <span>
#include <string>
#include <vector>

enum class CartridgeType
{
	NES,
	Unknown,
};

/**
 * CartridgeData - loads a cartridge file (ROM) and manages its
 * persistent state, stored next to the rom as <rom>.sav
 *
 * The .sav file is a small container holding sections with
 * different lifetimes:
 *   - battery state:   battery-backed cart memory (auto-saved on eject)
 *   - machine snapshot: last user save (durable, restored on request)
 * See CartridgeData.cpp for the format.
 */
class CartridgeData
{
public:
	// Loading data
	bool Load(const char* inFilename);
	void Clear();

	// Properties
	const std::string& GetFilename() const { return mFilename; }
	CartridgeType	   GetType() const { return mType; }

	// Data access
	uint32		 GetSize() const { return mData.size(); }
	const uint8* GetData() const { return mData.data(); }

	// Battery state (battery-backed cartridge memory)
	void SetPersistentState(std::span<uint8> inState) const;
	bool GetPersistentState(std::span<uint8>& outState) const;

	// Machine snapshot (full core state, core-specific layout)
	void SetSnapshot(std::span<const uint8> inState) const;
	bool GetSnapshot(std::span<const uint8>& outState) const;
	bool HasSnapshot() const { return !mSnapshotState.empty(); }
	void ClearSnapshot() const;

	// Write the .sav file (battery + snapshot sections)
	void SavePersistentState() const;

private:
	bool LoadSaveData();

	std::string				   mFilename;
	CartridgeType			   mType = CartridgeType::Unknown;

	std::vector<uint8>		   mData;
	mutable std::vector<uint8> mBatteryState;
	mutable std::vector<uint8> mSnapshotState;
};
