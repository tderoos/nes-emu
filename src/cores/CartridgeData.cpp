#include "CartridgeData.h"


bool CartridgeData::Load(const char* inFilename)
{
	// Clear old data
	Clear();

	// Open rom file
	FILE* fd = fopen(inFilename, "rb");
	if (fd == nullptr)
		return false;

	// Determine filesize
	fseek(fd, 0L, SEEK_END);
	const size_t size = ftell(fd);
	fseek(fd, 0l, SEEK_SET);

	// Allocate buffer
	void* data = malloc(size);
	if (data == nullptr)
		return false;

	const size_t num_read = fread(data, 1, size, fd);
	fclose(fd);

	// Check we read expected data
	if (num_read < size)
	{
		free(data);
		return false;
	}

	// Store values
	mData = static_cast<uint8*>(data);
	mSize = size;
	mFilename = inFilename;
	mType = CartridgeType::NES;
	return true;
}

void CartridgeData::Clear()
{
	// Free data
	if (mData) free((void*)mData);

	// Set defaults
	*this = CartridgeData();
}