// Frontend
#pragma once

#include <atomic>
#include <cstdint>

#include <SDL3/SDL.h>

#include "cartridge/CartridgeData.h"
#include "common/Mutex.h"
#include "common/RingBuffer.h"
#include "common/Thread.h"
#include "common/types.h"

class NES;

class Frontend
{
public:
	// Init exit
	SDL_AppResult Init();
	void		  Exit();
	virtual ~Frontend() { Exit(); }

	// Injecting / ejecting ROM
	virtual void InsertRom(const char* inPath);
	void		 EjectRom();

	// Operations
	bool				  ShowFileDialog();
	virtual void		  LoadInitialRom();
	virtual SDL_AppResult Iterate();
	void				  RunFrame();
	void				  Present() const;

	virtual SDL_AppResult OnSDLEvent(const SDL_Event& inEvent);

	// Write battery state + snapshot to the .sav now (not just on eject),
	// then run the platform flush hook. Needed on platforms without a
	// clean exit (web: tab close).
	void FlushSaveData();

protected:
	// Platform hooks: desktop runs the emulator on its own thread paced by
	// the audio device; the web frontend (WebFrontend) runs single-threaded.
	virtual void StartEmulation();
	virtual void StopEmulation();
	virtual void OnFrameReady(const uint32* inPixelBuffer, const uint8* inAudioBuffer, uint32 inAudioLength);
	virtual void OnSaveDataStored() {}

	void ProcessSaveStateRequest();

	// Callbacks
	static void sAudioDataRequested(void* inUserData, SDL_AudioStream* inStream, int inAdditionalAmount,
									int inTotalAmount);
	void		EmulatorThreadFunction();

	uint8		GetControllerData() const;

protected:
	enum class ESaveStateRequest
	{
		None,
		Load,
		Save
	};
	using LoadSaveRequest = std::atomic<ESaveStateRequest>;
	using FrameBuffer = uint32[256 * 256];
	using AudioBuffer = RingBuffer<uint8, 4096>;

	CartridgeData	  mCartridge;

	NES*			  mNES = nullptr;
	LoadSaveRequest	  mSaveStateRequest = ESaveStateRequest::None;

	Thread			  mEmulatorThread;
	SDL_Semaphore*	  mFrameSem = nullptr;
	SDL_Window*		  mWindow = nullptr;
	SDL_Renderer*	  mRenderer = nullptr;
	SDL_Texture*	  mTexture = nullptr;
	SDL_AudioDeviceID mAudioDevice = 0;
	SDL_AudioStream*  mAudioStream = nullptr;

	SDL_Gamepad*	  mPad = nullptr;
	Mutex			  mPadMutex;

	FrameBuffer		  mFrameBuffer = {0};
	Mutex			  mFrameBufferMutex;

	AudioBuffer		  mAudioBuffer;
	Mutex			  mAudioBufferMutex;
};
