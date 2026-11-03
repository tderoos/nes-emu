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
	using RomData = std::vector<uint8>;

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
	void SetPersistentState(StateDataSpan inState) const;
	bool GetPersistentState(StateDataSpan& outState) const;

	// Load/Save state
	LoadState GetLoadState() const;
	SaveState GetSaveState();

	// Write the .sav file (battery + snapshot sections)
	void StoreSaveData() const;

private:
	// Load state from .sav
	bool LoadSaveData();

	// Data
	std::string		 mFilename;
	CartridgeType	 mType = CartridgeType::Unknown;
	RomData			 mData;
	mutable StateData mBatteryState;
	StateData		 mSnapshotState;
};
