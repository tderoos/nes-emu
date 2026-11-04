// WebFrontend - Emscripten frontend: a single-threaded Frontend subclass.
// Emulation runs on the main thread, paced by a time accumulator - no
// threads/SharedArrayBuffer, so no COOP/COEP headers are needed and the
// page runs on any static host.
#pragma once

#include <string>

#include "Frontend.h"

class WebFrontend : public Frontend
{
public:
	SDL_AppResult Iterate() override;
	void		  LoadInitialRom() override;
	void		  InsertRom(const char* inPath) override;
	SDL_AppResult OnSDLEvent(const SDL_Event& inEvent) override;

	// Startup helpers: browsers only allow programmatic file pickers and
	// media from a user gesture, so these are deferred to the main loop /
	// the first input event instead of being called during SDL_AppInit.
	void QueueRomLoad(const char* inPath);
	void RequestFileDialog();

protected:
	void StartEmulation() override;
	void StopEmulation() override;
	void OnFrameReady(const uint32* inPixelBuffer, const uint8* inAudioBuffer, uint32 inAudioLength) override;
	void OnSaveDataStored() override;

private:
	double		mFrameAccum		  = 0.0;
	uint64		mLastTicksNS	  = 0;
	uint32		mFramesSinceFlush = 0;
	std::string mPendingRomPath;
	bool		mWantFileDialog	  = false;
};
