// nes-emu frontend
#pragma once

#include <atomic>
#include <cstdint>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

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
	~Frontend() { Exit(); }

	// Injecting / ejecting ROM
	void InsertRom(const char* inPath);
	void EjectRom();

	// Operations
	bool		  ShowFileDialog();
	void		  RunFrame();
	void		  Present() const;

	SDL_AppResult OnSDLEvent(const SDL_Event& inEvent);

private:
	// Callbacks
	static void sAudioDataRequested(void* inUserData, SDL_AudioStream* inStream, int inAdditionalAmount,
									int inTotalAmount);
	void		EmulatorThreadFunction();

	uint8		GetControllerData() const;

private:
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
