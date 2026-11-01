#include "CartridgeData.h"

#include <cstdio>

static bool sReadFile(const std::string& inFilename, std::vector<uint8>& outData)
{
	// Open file
	FILE* fd = fopen(inFilename.c_str(), "rb");
	if (fd == nullptr)
		return false;

	// Determine filesize
	fseek(fd, 0L, SEEK_END);
	const size_t size = ftell(fd);
	fseek(fd, 0l, SEEK_SET);

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



bool CartridgeData::Load(const char* inFilename)
{
	// Clear old data
	Clear();

	if (!sReadFile(inFilename, mData))
		return false;

	// Store values
	mFilename = inFilename;
	mType = CartridgeType::NES;

	// Try to read persistent state data
	if (!sReadFile(mFilename + ".sav", mPersistentState))
		mPersistentState.clear();

	return true;
}

void CartridgeData::SetPersistentState(std::span<uint8> inState) const
{
	mPersistentState.assign(inState.begin(), inState.end());
}

bool CartridgeData::GetPersistentState(std::span<uint8>& outState) const
{
	if (mPersistentState.empty())
		return false;

	outState = mPersistentState;
	return true;
}


void CartridgeData::SavePersistentState() const
{
	std::string save_filename = mFilename + ".sav";
	FILE* savegame = fopen(save_filename.c_str(), "w");
	if (savegame != nullptr)
	{
		fwrite(mPersistentState.data(), 1, mPersistentState.size(), savegame);
		fclose(savegame);
	}
}


void CartridgeData::Clear()
{
	// Set defaults
	*this = CartridgeData();
}
