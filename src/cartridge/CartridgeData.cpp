#include "CartridgeData.h"

#include <cstring>
#include <filesystem>

using std::filesystem::remove;
using std::filesystem::rename;

// .sav container format (little-endian):
//   [magic   8 bytes]  "EMUSAVE\0"
//   [version 4 bytes]  container format version (1)
//   [flags   4 bytes]  bit0: battery section present
//                       bit1: snapshot section present
//   [battery section]  [size 4 bytes][data]     (if bit0)
//   [snapshot section] [size 4 bytes][data]    (if bit1)
// The snapshot blob is core-specific: its first field is the core's own
// state version, written/checked by the core (e.g. NES), not by this container.
// A file without the magic is not a valid .sav and is reported as failed.

static constexpr char	sMagic[8] = {'E', 'M', 'U', 'S', 'A', 'V', 'E', '\0'};
static constexpr uint32 sVersion = 1;
static constexpr uint32 sFlagBattery = 1 << 0;
static constexpr uint32 sFlagSnapshot = 1 << 1;

static CartridgeType sGetCartridgeType(std::span<const uint8> inData)
{
	// Validate the iNES header (16 bytes header, "NES\x1A" magic).
	static constexpr char sNesMagic[4] = {'N', 'E', 'S', '\x1A'};
	if (inData.size() >= 16 && memcmp(inData.data(), sNesMagic, sizeof(sNesMagic)) == 0)
		return CartridgeType::NES;

	return CartridgeType::Unknown;
}

struct SaveDataHeader
{
	SaveDataHeader()
	{
		memcpy(mMagic, sMagic, sizeof(sMagic));
		mVersion = sVersion;
		mFlags = 0;
	}
	bool   IsValid() const { return memcmp(mMagic, sMagic, sizeof(sMagic)) == 0 && mVersion == sVersion; }

	char   mMagic[8] = {};
	uint32 mVersion;
	uint32 mFlags;
};

/**
 * @brief sReadFile
 * @param inFilename Filename to rad
 * @param outData Data read from file
 * @return true if data was read successfully.
 */
static bool sReadFile(const std::string& inFilename, std::vector<uint8>& outData)
{
	// Open file
	FILE* fd = fopen(inFilename.c_str(), "rb");
	if (fd == nullptr)
		return false;

	// Determine filesize
	fseek(fd, 0L, SEEK_END);
	const size_t size = ftell(fd);
	fseek(fd, 0L, SEEK_SET);

	// Resize buffer
	std::vector<uint8> buffer;
	buffer.resize(size);

	const size_t num_read = fread(buffer.data(), 1, size, fd);
	fclose(fd);

	// Check we read expected data
	if (num_read < size)
		return false;

	outData.swap(buffer);
	return true;
}

/**
 *
 * @param inFilename
 * @return
 */
bool CartridgeData::Load(const char* inFilename)
{
	// Clear old data
	Clear();

	fprintf(stderr, "Loading %s", inFilename);

	if (!sReadFile(inFilename, mData))
	{
		fprintf(stderr, "Failed to load '%s'\n", mFilename.c_str());
		return false;
	}

	// Store values
	mFilename = inFilename;
	mType = sGetCartridgeType(mData);

	if (mType == CartridgeType::Unknown)
	{
		fprintf(stderr, "'%s' is not a support ROM format\n", inFilename);
		return false;
	}

	// Try to read persistent state data. A missing .sav is fine; a corrupt
	// or incompatible one is logged but must not fail loading the cartridge.
	if (!LoadSaveData())
		fprintf(stderr, "Failed to load '%s.sav' (corrupt or unsupported), ignoring it\n", mFilename.c_str());

	return true;
}

LoadState CartridgeData::GetLoadState() const
{
	return LoadState(mSnapshotState);
}

SaveState CartridgeData::GetSaveState()
{
	mSnapshotState.clear();
	return SaveState(mSnapshotState);
}

// Persistent (memory backed) cartridge state
LoadState CartridgeData::GetPersistentLoadState() const
{
	return LoadState(mBatteryState);
}

SaveState CartridgeData::GetPersistentSaveState()
{
	mBatteryState.clear();
	return SaveState(mBatteryState);
}

void CartridgeData::StoreSaveData() const
{
	// Serialize the container
	std::vector<uint8> out;
	out.reserve(sizeof(SaveDataHeader) + 8 + mBatteryState.size() + mSnapshotState.size());

	auto append = [&out](const void* inData, size_t inSize)
	{
		const uint8* data = static_cast<const uint8*>(inData);
		out.insert(out.end(), data, data + inSize);
	};

	// Append header
	SaveDataHeader header;
	header.mFlags = (mBatteryState.empty() ? 0 : sFlagBattery) | (mSnapshotState.empty() ? 0 : sFlagSnapshot);
	append(&header, sizeof(SaveDataHeader));

	// Append save sections
	auto append_section = [&append](const StateData& inSection)
	{
		const uint32 size = static_cast<uint32>(inSection.size());
		append(&size, sizeof(size));
		append(inSection.data(), size);
	};

	if (header.mFlags & sFlagBattery)
		append_section(mBatteryState);

	if (header.mFlags & sFlagSnapshot)
		append_section(mSnapshotState);

	// Write atomically: a crash mid-write never destroys the previous
	// file (which may hold battery saves that are precious).
	const std::string save_filename = mFilename + ".sav";
	const std::string temp_filename = save_filename + ".tmp";
	if (FILE* fd = fopen(temp_filename.c_str(), "wb"); fd != nullptr)
	{
		const size_t written = fwrite(out.data(), 1, out.size(), fd);
		fclose(fd);
		if (written == out.size())
			rename(temp_filename, save_filename);
		else
			remove(temp_filename);
	}
}


/**
 * @brief Load the <rom>.sav container into mBatteryState / mSnapshotState
 * @return true if there is no .sav or it parsed successfully, false on any parse failure.
 */
bool CartridgeData::LoadSaveData()
{
	std::vector<uint8> save_data;
	if (!sReadFile(mFilename + ".sav", save_data))
		return true; // no save file yet: nothing to load

	auto iter = save_data.begin();
	auto read_range = [&iter, end = save_data.end()](uint8* outDest, uint32 inSize)
	{
		if (iter + inSize > end)
			return false;
		memcpy(outDest, &(*iter), inSize);
		iter += inSize;
		return true;
	};

	auto read_value = [&read_range]<typename taType>(taType& outValue) -> bool
	{
		return read_range(reinterpret_cast<uint8*>(&outValue), sizeof(taType));
	};

	SaveDataHeader header;
	if (!read_value(header) || !header.IsValid())
		return false;

	auto read_save_section = [&read_value, &read_range](StateData& outData)
	{
		uint32 section_size;
		if (!read_value(section_size))
			return false;

		outData.resize(section_size);
		return read_range(outData.data(), section_size);
	};

	if (header.mFlags & sFlagBattery)
		if (!read_save_section(mBatteryState))
			return false;

	if (header.mFlags & sFlagSnapshot)
		if (!read_save_section(mSnapshotState))
			return false;

	return true;
}


void CartridgeData::Clear()
{
	// Set defaults
	*this = CartridgeData();
}
