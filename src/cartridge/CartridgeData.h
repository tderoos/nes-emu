#pragma once
#include "common/SaveState.h"
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

	// ROM Data access
	std::span<const uint8> GetRom() const { return mData; }

	// Load/Save state
	LoadState GetLoadState() const;
	SaveState GetSaveState();

	// Persistent (battery backed) cartridge state
	LoadState GetPersistentLoadState() const;
	SaveState GetPersistentSaveState();

	// Write the .sav file (battery + snapshot sections)
	void StoreSaveData() const;

private:
	// Load state from .sav
	bool LoadSaveData();

	// Data
	std::string		   mFilename;
	CartridgeType	   mType = CartridgeType::Unknown;
	std::vector<uint8> mData;
	StateData		   mBatteryState;
	StateData		   mSnapshotState;
};
