// WebFrontend - Emscripten frontend: a single-threaded Frontend subclass.
// Emulation runs on the main thread, paced by a time accumulator - no
// threads/SharedArrayBuffer, so no COOP/COEP headers are needed and the
// page runs on any static host.
#pragma once

#include "Frontend.h"

class WebFrontend : public Frontend
{
public:
	SDL_AppResult Iterate() override;
	void		  LoadInitialRom() override;

protected:
	void StartEmulation() override;
	void StopEmulation() override;
	void OnFrameReady(const uint32* inPixelBuffer, const uint8* inAudioBuffer, uint32 inAudioLength) override;
	void OnSaveDataStored() override;

private:
	double mFrameAccum	= 0.0;
	uint64 mLastTicksNS	= 0;
};
