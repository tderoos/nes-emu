// nes-emu main

#include <semaphore>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_mutex.h>
#include <SDL3/SDL_thread.h>
#include <SDL3/SDL_joystick.h>
#include "NES/NES.h"
#include "NES/types.h"



static SDL_AppResult SDL_Fail()
{
    SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "Error %s", SDL_GetError());
    return SDL_APP_FAILURE;
}

template <typename taType>
static void SafeDelete(taType* &inPtr) {
	if (!inPtr)
		return;

	delete inPtr;
	inPtr = nullptr;
}


class Mutex
{
public:
					Mutex()				{ mMutex = SDL_CreateMutex(); }
					~Mutex()			{ SDL_DestroyMutex(mMutex); }

	void			Lock() const		{ SDL_LockMutex(mMutex); }
	void			Unlock() const		{ SDL_UnlockMutex(mMutex); }

private:
	SDL_Mutex*		mMutex = nullptr;
};


template <typename taType, size_t taSize>
class RingBuffer
{
public:
	static constexpr size_t sMaxSize = taSize;

	int		Push(const taType* inData, uint32 inCount)
	{
		mMutex.Lock();

		const uint32 count = std::min<uint32>(inCount, taSize - mSize);
		const uint32 size1 = std::min<uint32>(count, taSize - mWriteIndex);
		const uint32 size2 = count - size1;

		memcpy(&mData[mWriteIndex], inData, size1);
		mWriteIndex += size1;
		if (size2) {
			memcpy(&mData[0], inData+size1, size2);
			mWriteIndex = size2;
		}

		mSize += count;
		mMutex.Unlock();

		return count;
	}

	int		Pop(taType* outData, uint32 inCount)
	{
		mMutex.Lock();

		const uint32 count = std::min<uint32>(inCount, mSize);
		const uint32 size1 = std::min<uint32>(count, taSize - mReadIndex);
		const uint32 size2 = count - size1;

		memcpy(outData, &mData[mReadIndex], size1);
		mReadIndex += size1;
		if (size2) {
			memcpy(outData + size1, &mData[0], size2);
			mReadIndex = size2;
		}

		mSize -= count;
		mMutex.Unlock();

		return count;
	}

	void Clear() {
		mMutex.Lock();
		mSize = mReadIndex = mWriteIndex = 0;
		mMutex.Unlock();
	}

	Mutex	mMutex;

	int		mSize = 0;
	int		mReadIndex = 0;
	int		mWriteIndex = 0;
	taType	mData[taSize] = {0};
};

/**
 * Thread helper object
 */
class Thread
{
public:
					Thread() = default;
					~Thread() { Destroy(); }

	using ThreadFunc = int (*) (void *data);

	static void		sCreateThread(Thread& ioThread, ThreadFunc inFunc, const char* inName, void* inUserData = nullptr)
	{
		struct sdl_thread_context
		{
			Thread*		mThread;
			ThreadFunc	mFunc;
			void*		mUserData;
		};

		auto sdl_thread_funct = [](void* inContext)
		{
			auto ctx = static_cast<sdl_thread_context*>(inContext);
			sCurrentThread = ctx->mThread;
			const int val = ctx->mFunc(ctx->mUserData);
			sCurrentThread = nullptr;
			delete ctx;
			return val;
		};

		ioThread.mExit   = false;
		ioThread.mThread = SDL_CreateThread(sdl_thread_funct, inName, new sdl_thread_context(&ioThread, inFunc, inUserData));
	}

	void			Destroy()
	{
		if (!mThread)
			return;

		mExit = true;
		SDL_WaitThread(mThread, nullptr);
		mThread = nullptr;
	}

	bool			IsExit() const				{ return mExit; }
	static const Thread* sGetCurrentThread()	{ return sCurrentThread; }

private:
	static thread_local	Thread* sCurrentThread;
	bool						mExit   = false;
	SDL_Thread*					mThread = nullptr;
};

thread_local Thread* Thread::sCurrentThread;

/**
 *
 */
class NESApp
{
public:
	enum class ESaveStateRequest
	{
		None, Load, Save
	};

	// Init exit
    SDL_AppResult	Init();
    void			Exit();
					~NESApp()												{ Exit(); }

	// Injecting / ejecting ROM
	void			InsertRom(const char* inPath);
	void			EjectRom();

	// Operations
	bool			ShowFileDialog();
    void			RunFrame();
	void			Present() const;

	SDL_AppResult	OnSDLEvent(const SDL_Event& inEvent);

private:
    // Callbacks
    static void		AudioDataRequested(void* userdata, SDL_AudioStream* stream, int additional_amount, int total_amount);
	void			EmulatorThreadFunction();

	uint8			GetControllerData();

private:
    using NESFrameBuffer = uint32[256*256];
	using AudioBuffer	 = RingBuffer<uint8, 4096>;

    SDL_Semaphore*		mFrameSem = nullptr;
    SDL_Window*			mWindow = nullptr;
    SDL_Renderer*		mRenderer = nullptr;
    SDL_Texture*		mTexture = nullptr;
    SDL_AudioDeviceID	mAudioDevice = 0;
    SDL_AudioStream*	mAudioStream = nullptr;
    NES*				mNES = nullptr;
	Thread				mEmulatorThread;

	SDL_Joystick*		mStick = nullptr;
	Mutex				mStickMutex;

    SaveState			mSaveState;
    ESaveStateRequest	mSaveStateRequest = ESaveStateRequest::None;

    Mutex				mFrameBufferMutex;
    NESFrameBuffer      mFrameBuffer = {0};

	AudioBuffer			mAudioBuffer;
};


SDL_AppResult NESApp::Init()
{
    // init the library, here we make a window so we only need the Video capabilities.
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD | SDL_INIT_EVENTS))
        return SDL_Fail();

    // create a window
    mWindow = SDL_CreateWindow("NES", 512, 512, SDL_WINDOW_RESIZABLE);
    if (!mWindow)
        return SDL_Fail();

    mRenderer = SDL_CreateRenderer(mWindow, nullptr);
    if (!mRenderer)
        return SDL_Fail();

    mTexture = SDL_CreateTexture(mRenderer, SDL_PIXELFORMAT_XRGB8888, SDL_TEXTUREACCESS_STREAMING, 256, 256);
    if (!mTexture)
        return SDL_Fail();

	mFrameSem = SDL_CreateSemaphore(0);

	// vsync paces presentation to the display refresh
	SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");

	SDL_AudioSpec want;
	SDL_memset(&want, 0, sizeof(want)); /* or SDL_zero(want) */
	want.format = SDL_AUDIO_S8;
	want.channels = 1;
	want.freq = 44100;

	SDL_AudioSpec src;
	SDL_memset(&src, 0, sizeof(src)); /* or SDL_zero(want) */
	src.format = SDL_AUDIO_S8;
	src.channels = 1;
	src.freq = 44056;

    // The audio device pulls the emulator: its get-callback fires on the
    // audio thread whenever the device needs more data, waking the main
    // thread to run exactly one frame. The sound card's clock is the master.
    mAudioDevice = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &want);
    mAudioStream = SDL_CreateAudioStream(&src, &want);
	SDL_PauseAudioStreamDevice(mAudioStream);

    // Show window
    SDL_ShowWindow(mWindow);

    SDL_Log("Application started successfully!");

    return SDL_APP_CONTINUE;
}


void NESApp::Exit()
{
	EjectRom();

	if (mAudioStream != nullptr)
		SDL_DestroyAudioStream(mAudioStream);
	mAudioStream = nullptr;

	if (mAudioDevice != 0)
		SDL_CloseAudioDevice(mAudioDevice);
	mAudioDevice = 0;
}


// Injecting / ejecting ROM
void NESApp::InsertRom(const char* inPath)
{
	if (mNES != nullptr)
		EjectRom();

	mNES = new NES(inPath);
	Thread::sCreateThread(mEmulatorThread, [](void* appstate){ ((NESApp*) appstate)->EmulatorThreadFunction(); return 0; }, "EmulatorThread", this);
}



void NESApp::EjectRom()
{
	if (mNES != nullptr)
	{
		// Exit the emulator thread
		mEmulatorThread.Destroy();
		SafeDelete(mNES);
	}
}


bool NESApp::ShowFileDialog()
{
	// ask the user which ROM to load
	static const SDL_DialogFileFilter filters[] =
	{
		{ "NES ROM", "nes" },
		{ "All files", "*" }
	};

	auto rom_selected = [](void* userdata, const char* const* filelist, int filter)
	{
		if (*filelist)
			static_cast<NESApp*>(userdata)->InsertRom(*filelist);
	};

	SDL_ShowOpenFileDialog(rom_selected, this, mWindow, filters, 2, nullptr, false);
	return true;
}


/**
 * @return
 */
void NESApp::EmulatorThreadFunction()
{
	// Prime the stream: run one frame immediately so the audio device
	// has data on its very first request, and the callback never starts
	// out waiting on the emulator.
	RunFrame();

	// Resume the audio device so it start consuming.
	SDL_SetAudioStreamGetCallback(mAudioStream, AudioDataRequested, this);
	SDL_BindAudioStream(mAudioDevice, mAudioStream);
	SDL_ResumeAudioStreamDevice(mAudioStream);

	// Continue emulating until quit is requested
	while (!Thread::sGetCurrentThread()->IsExit())
	{
		// Run a frame
		if (SDL_WaitSemaphoreTimeout(mFrameSem, 100))
			RunFrame();

		// Handle load/save state request
		switch (mSaveStateRequest)
		{
			case ESaveStateRequest::None:	break;
			case ESaveStateRequest::Load:	mNES->ReadState(mSaveState); break;
			case ESaveStateRequest::Save:	mNES->WriteState(mSaveState); break;
		}
		mSaveStateRequest = ESaveStateRequest::None;
	}

	// Pause the audio device until the emulator is running again.
	SDL_PauseAudioStreamDevice(mAudioStream);
	SDL_UnbindAudioStream(mAudioStream);
	SDL_SetAudioStreamGetCallback(mAudioStream, nullptr, nullptr);
	SDL_ClearAudioStream(mAudioStream);

	mAudioBuffer.Clear();
}


uint8 NESApp::GetControllerData()
{
	uint8 controller_input = 0;

	mStickMutex.Lock();
	if (mStick)
	{
		controller_input |= SDL_GetJoystickButton(mStick, 0) << 0;
		controller_input |= SDL_GetJoystickButton(mStick, 1) << 1;
		controller_input |= SDL_GetJoystickButton(mStick, 4) << 2;
		controller_input |= SDL_GetJoystickButton(mStick, 6) << 3;

		uint8 hat = SDL_GetJoystickHat(mStick, 0);
		controller_input |= (hat & 1) ? 1 << 4 : 0;
		controller_input |= (hat & 4) ? 1 << 5 : 0;
		controller_input |= (hat & 8) ? 1 << 6 : 0;
		controller_input |= (hat & 2) ? 1 << 7 : 0;
	}
	mStickMutex.Unlock();

	return controller_input;
}




// Emulate one frame, publish the video, and supply its audio to the stream.
// Called on the main thread: once at ROM load (to prime the stream) and once
// per watermark signal from the audio callback thereafter.
void NESApp::RunFrame()
{
	uint8 audio_buffer[768];
	uint32 audio_buffer_length = 0;
//	memset(audio_buffer, 127, 768);

	NESFrameBuffer pixel_buffer;
	mNES->RunToVBlank(GetControllerData(), pixel_buffer, audio_buffer, &audio_buffer_length);

	mAudioBuffer.Push(audio_buffer, audio_buffer_length);

	mFrameBufferMutex.Lock();
	memcpy(mFrameBuffer, pixel_buffer, sizeof(NESFrameBuffer));
	mFrameBufferMutex.Unlock();
}


void NESApp::Present() const
{
	NESFrameBuffer pixel_buffer;

	mFrameBufferMutex.Lock();
	memcpy(pixel_buffer, mFrameBuffer, sizeof(NESFrameBuffer));
	mFrameBufferMutex.Unlock();

	void* pixels;
	int pitch;
	SDL_LockTexture(mTexture, nullptr, &pixels, &pitch);
	for (int i = 0; i < 256; i++)
		memcpy((uint8*) pixels + i * pitch, pixel_buffer + i * 256, 256*sizeof(uint32));
	SDL_UnlockTexture(mTexture);

	SDL_RenderTexture(mRenderer, mTexture, nullptr, nullptr);
	SDL_RenderPresent(mRenderer);
}


SDL_AppResult NESApp::OnSDLEvent(const SDL_Event& inEvent)
{
	const SDL_EventType event_type = (SDL_EventType) inEvent.type;

	// Handle keyboard events
	if (event_type == SDL_EVENT_KEY_UP)
	{
		if (inEvent.key.scancode == SDL_SCANCODE_K)
			mSaveStateRequest = ESaveStateRequest::Save;
		else if(inEvent.key.scancode == SDL_SCANCODE_L)
			mSaveStateRequest = ESaveStateRequest::Load;
	}

	if (event_type == SDL_EVENT_JOYSTICK_ADDED)
	{
		mStickMutex.Lock();

		if (mStick == nullptr)
		{
			int count;
			SDL_JoystickID* sticks = SDL_GetJoysticks(&count);

			if (sticks && count > 0)
				mStick = SDL_OpenJoystick(sticks[0]);
			SDL_free(sticks);
		}

		mStickMutex.Unlock();
	}
	if (event_type == SDL_EVENT_JOYSTICK_REMOVED)
	{
		mStickMutex.Lock();

		if (mStick != nullptr && SDL_GetJoystickID(mStick) == inEvent.jdevice.which)
		{
			mStick = nullptr;
			int count;
			SDL_JoystickID* sticks = SDL_GetJoysticks(&count);

			if (sticks && count > 0)
				mStick = SDL_OpenJoystick(sticks[0]);
			SDL_free(sticks);
		}

		mStickMutex.Unlock();
	}

	return SDL_APP_CONTINUE;
}



// Called on the audio device's own thread, right before it consumes data.
// The emulator runs one frame AHEAD: the stream always holds >= 1 frame of
// audio, so the callback can consume immediately - it never waits for the
// emulator. The signal is a watermark: when less than a frame's worth is
// left queued, tell the main thread to emulate the next frame. That keeps
// supply locked to the sound card's consumption rate (master clock), with
// ~1 frame of latency.
void NESApp::AudioDataRequested(void* userdata, SDL_AudioStream* stream, int additional_amount, int total_amount)
{
	auto app = static_cast<NESApp*>(userdata);

    constexpr int kFrameAudioBytes = 736;   // 44056Hz / 60.0988fps, mono S8

	uint8 buffer[AudioBuffer::sMaxSize];
	const int sample_count = app->mAudioBuffer.Pop(buffer, total_amount);
	SDL_PutAudioStreamData(app->mAudioStream, buffer, sample_count);

	if (sample_count < additional_amount)
		printf("underflow (%d - %d - %d - %d)\n", total_amount, additional_amount, sample_count, app->mAudioBuffer.mSize);

	int cur_size = app->mAudioBuffer.mSize;
	while (cur_size < 1.5 * kFrameAudioBytes) {
		SDL_SignalSemaphore(app->mFrameSem);
		cur_size+= kFrameAudioBytes;
	}
}


SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
    auto app = new NESApp();
    if (SDL_AppResult result = app->Init(); result != SDL_APP_CONTINUE)
    {
        delete app;
        return result;
    }

	// Open the file dialog
	app->ShowFileDialog();

    // Set the app the the SDL app state and continue execution
    *appstate = app;
    return SDL_APP_CONTINUE;
}



SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event* inEvent)
{
	auto app = static_cast<NESApp*>(appstate);
	const SDL_EventType event_type = (SDL_EventType) inEvent->type;

	// Handle quit event
    if (event_type == SDL_EVENT_QUIT)
    	return SDL_APP_SUCCESS;

	return app->OnSDLEvent(*inEvent);

    // Handle the window close button
    // if (event->type == SDL_EVENT_WINDOW_CLOSE_REQUESTED
    //     && event->window.windowID == SDL_GetWindowID(app->mWindow))
    // 	return SDL_APP_SUCCESS;
}



SDL_AppResult SDL_AppIterate(void* appstate)
{
	auto app = static_cast<NESApp*>(appstate);
	app->Present();
	return SDL_APP_CONTINUE;
}


void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
    if (auto* app = static_cast<NESApp*>(appstate))
    {
    	app->Exit();
	    delete app;
    }

    SDL_Quit();
    SDL_Log("Application quit successfully!");
}
