#include "CartridgeData.h"

#include <cstdio>
#include <cstring>

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

	if (!sReadFile(inFilename, mData))
		return false;

	// Store values
	mFilename = inFilename;
	mType = CartridgeType::NES;

	// Try to read persistent state data. A missing .sav is fine; a corrupt
	// or incompatible one is logged but must not fail loading the cartridge.
	if (!LoadSaveData())
		fprintf(stderr, "CartridgeData: failed to load '%s.sav' (corrupt or unsupported), ignoring it\n",
				mFilename.c_str());

	return true;
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

	// Parse the container
	const size_t header_size = 8 + 4 + 4;
	if (save_data.size() < header_size || memcmp(save_data.data(), sMagic, sizeof(sMagic)) != 0)
		return false;

	size_t offset = sizeof(sMagic);
	uint32 version, flags;
	memcpy(&version, &save_data[offset], sizeof(version));
	offset += 4;
	memcpy(&flags, &save_data[offset], sizeof(flags));
	offset += 4;
	if (version != sVersion)
		return false; // future versions: refuse rather than misinterpret

	for (uint32 bit : {sFlagBattery, sFlagSnapshot})
	{
		if ((flags & bit) == 0)
			continue;

		uint32 section_size;
		if (offset + 4 > save_data.size())
			return false;
		memcpy(&section_size, &save_data[offset], 4);
		offset += 4;
		if (offset + section_size > save_data.size())
			return false;

		std::span<const uint8> section(save_data.data() + offset, section_size);
		if (bit == sFlagBattery)
			mBatteryState.assign(section.begin(), section.end());
		else
			mSnapshotState.assign(section.begin(), section.end());
		offset += section_size;
	}

	return true;
}

void CartridgeData::SetPersistentState(std::span<uint8> inState) const
{
	mBatteryState.assign(inState.begin(), inState.end());
}

bool CartridgeData::GetPersistentState(std::span<uint8>& outState) const
{
	if (mBatteryState.empty())
		return false;

	outState = mBatteryState;
	return true;
}

void CartridgeData::SetSnapshot(std::span<const uint8> inState) const
{
	mSnapshotState.assign(inState.begin(), inState.end());
}

bool CartridgeData::GetSnapshot(std::span<const uint8>& outState) const
{
	if (mSnapshotState.empty())
		return false;

	outState = mSnapshotState;
	return true;
}

void CartridgeData::ClearSnapshot() const
{
	mSnapshotState.clear();
}

void CartridgeData::SavePersistentState() const
{
	// Serialize the container
	std::vector<uint8> out;
	out.reserve(sizeof(sMagic) + 12 + 8 + mBatteryState.size() + mSnapshotState.size());

	auto append = [&out](const void* inData, size_t inSize)
	{ out.insert(out.end(), static_cast<const uint8*>(inData), static_cast<const uint8*>(inData) + inSize); };

	uint32 flags = (mBatteryState.empty() ? 0 : sFlagBattery) | (mSnapshotState.empty() ? 0 : sFlagSnapshot);
	append(sMagic, sizeof(sMagic));
	append(&sVersion, sizeof(sVersion));
	append(&flags, sizeof(flags));

	for (const std::vector<uint8>* section : {&mBatteryState, &mSnapshotState})
	{
		if (section->empty())
			continue;

		uint32 size = static_cast<uint32>(section->size());
		append(&size, sizeof(size));
		append(section->data(), size);
	}

	// Write atomically: a crash mid-write never destroys the previous
	// file (which may hold battery saves that are precious).
	const std::string save_filename = mFilename + ".sav";
	const std::string temp_filename = save_filename + ".tmp";
	if (FILE* fd = fopen(temp_filename.c_str(), "wb"); fd != nullptr)
	{
		const size_t written = fwrite(out.data(), 1, out.size(), fd);
		fclose(fd);
		if (written == out.size())
			rename(temp_filename.c_str(), save_filename.c_str());
		else
			remove(temp_filename.c_str());
	}
}

void CartridgeData::Clear()
{
	// Set defaults
	*this = CartridgeData();
}
