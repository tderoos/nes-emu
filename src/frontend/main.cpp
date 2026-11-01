// nes-emu main

#include <atomic>
#include <cstring>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "common/Mutex.h"
#include "common/RingBuffer.h"
#include "common/Thread.h"
#include "common/types.h"
#include "cartridge/CartridgeData.h"
#include "cores/nes/NES.h"
#include "cores/nes/SaveState.h"

static constexpr int kNESSampleFrequency = 44056;

static SDL_AppResult SDL_Fail()
{
	SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "Error %s", SDL_GetError());
	return SDL_APP_FAILURE;
}

template <typename taType>
static void SafeDelete(taType*& ioPtr)
{
	if (!ioPtr)
		return;

	delete ioPtr;
	ioPtr = nullptr;
}

/**
 *
 */
class NESApp
{
public:
	// Init exit
	SDL_AppResult Init();
	void		  Exit();
	~NESApp() { Exit(); }

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
	SaveState		  mSaveState;
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

/**
 *
 * @return
 */
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

	// vsync paces presentation to the display refresh
	SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
	SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");

	// Audio source format is 44056Hz, target format is 44100Hz
	constexpr SDL_AudioSpec src{.format = SDL_AUDIO_S8, .channels = 1, .freq = kNESSampleFrequency};
	constexpr SDL_AudioSpec want{.format = SDL_AUDIO_S8, .channels = 1, .freq = 44100};

	// Init audio device
	mAudioDevice = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &want);
	mAudioStream = SDL_CreateAudioStream(&src, &want);

	// Pause the stream until the emulator is running
	SDL_PauseAudioStreamDevice(mAudioStream);

	// Semaphore to trigger frame generation
	mFrameSem = SDL_CreateSemaphore(0);

	// Show window and start application
	SDL_ShowWindow(mWindow);
	SDL_Log("Application started successfully!");

	return SDL_APP_CONTINUE;
}

/**
 *
 */
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

/**
 *
 * @param inPath
 */
void NESApp::InsertRom(const char* inPath)
{
	if (mNES != nullptr)
		EjectRom();

	if (!mCartridge.Load(inPath))
		return;

	mNES = new NES(mCartridge);
	Thread::sCreateThread(
		mEmulatorThread,
		[](void* inAppState)
		{
			static_cast<NESApp*>(inAppState)->EmulatorThreadFunction();
			return 0;
		},
		"EmulatorThread", this);
}

/**
 *
 */
void NESApp::EjectRom()
{
	if (mNES != nullptr)
	{
		// Exit the emulator thread
		mEmulatorThread.Destroy();
		SafeDelete(mNES);

		// Save persistent game-state
		mCartridge.SavePersistentState();
	}

	mCartridge.Clear();
}

/**
 *
 * @return
 */
bool NESApp::ShowFileDialog()
{
	// ask the user which ROM to load
	static constexpr SDL_DialogFileFilter filters[] = {{"NES ROM", "nes"}, {"All files", "*"}};

	auto								  rom_selected = [](void* userdata, const char* const* filelist, int /*filter*/)
	{
		if (*filelist)
			static_cast<NESApp*>(userdata)->InsertRom(*filelist);
	};

	SDL_ShowOpenFileDialog(rom_selected, this, mWindow, filters, 2, nullptr, false);
	return true;
}

/**
 * @brief Thread function to run the emulator
 *
 * This function runs the emulator, processing controller input and outputting audio and frame data.
 * It is paced by the audio-buffer. The requests for audio data provide a steady clock to sync the emulator
 * that is more accurate the vsync.
 */
void NESApp::EmulatorThreadFunction()
{
	// Prime the stream: run one frame immediately so the audio device
	// has data on its very first request, and the callback never starts
	// out waiting on the emulator.
	RunFrame();

	// Resume the audio device so it start consuming.
	SDL_SetAudioStreamGetCallback(mAudioStream, sAudioDataRequested, this);
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
		case ESaveStateRequest::Load:
			mNES->ReadState(mSaveState);
			break;
		case ESaveStateRequest::Save:
			mNES->WriteState(mSaveState);
			break;
		default:
			break;
		}
		mSaveStateRequest = ESaveStateRequest::None;
	}

	// Pause the audio device until the emulator is running again.
	SDL_PauseAudioStreamDevice(mAudioStream);
	SDL_UnbindAudioStream(mAudioStream);
	SDL_SetAudioStreamGetCallback(mAudioStream, nullptr, nullptr);
	SDL_ClearAudioStream(mAudioStream);

	// Clear the audio buffer - safe since callback is disabled now.
	mAudioBuffer.Clear();
}

/**
 * @brief Get data from active controller
 * @return Bitfield containing button data
 */
uint8 NESApp::GetControllerData() const
{
	uint8 controller_input = 0;

	AutoLockMutex lock(mPadMutex);
	if (mPad)
	{
		controller_input |= SDL_GetGamepadButton(mPad, SDL_GAMEPAD_BUTTON_SOUTH) << 0; // A
		controller_input |= SDL_GetGamepadButton(mPad, SDL_GAMEPAD_BUTTON_EAST) << 1;  // B
		controller_input |= SDL_GetGamepadButton(mPad, SDL_GAMEPAD_BUTTON_BACK) << 2;  // Select
		controller_input |= SDL_GetGamepadButton(mPad, SDL_GAMEPAD_BUTTON_START) << 3; // Start

		controller_input |= SDL_GetGamepadButton(mPad, SDL_GAMEPAD_BUTTON_DPAD_UP) << 4;	// Up
		controller_input |= SDL_GetGamepadButton(mPad, SDL_GAMEPAD_BUTTON_DPAD_DOWN) << 5;	// Down
		controller_input |= SDL_GetGamepadButton(mPad, SDL_GAMEPAD_BUTTON_DPAD_LEFT) << 6;	// Left
		controller_input |= SDL_GetGamepadButton(mPad, SDL_GAMEPAD_BUTTON_DPAD_RIGHT) << 7; // Right
	}
	else
	{
		const bool* key_state = SDL_GetKeyboardState(nullptr);
		controller_input |= key_state[SDL_SCANCODE_K] << 0; // A
		controller_input |= key_state[SDL_SCANCODE_O] << 1;  // B
		controller_input |= key_state[SDL_SCANCODE_SPACE] << 2;  // Select
		controller_input |= key_state[SDL_SCANCODE_RETURN] << 3; // Start

		controller_input |= key_state[SDL_SCANCODE_W] << 4;	// Up
		controller_input |= key_state[SDL_SCANCODE_S] << 5;	// Down
		controller_input |= key_state[SDL_SCANCODE_A] << 6;	// Left
		controller_input |= key_state[SDL_SCANCODE_D] << 7; // Right
	}

	return controller_input;
}

/**
 * @brief Run a single frame, output generated data to frame and audio buffer.
 */
void NESApp::RunFrame()
{
	uint8		audio_buffer[768];
	uint32		audio_buffer_length = 0;
	FrameBuffer pixel_buffer;
	mNES->RunToVBlank(GetControllerData(), pixel_buffer, audio_buffer, &audio_buffer_length);

	{
		AutoLockMutex lock(mAudioBufferMutex);
		mAudioBuffer.Push(audio_buffer, audio_buffer_length);
	}
	{
		AutoLockMutex lock(mFrameBufferMutex);
		memcpy(mFrameBuffer, pixel_buffer, sizeof(FrameBuffer));
	}
}

/**
 * @brief Present the latest framebuffer
 */
void NESApp::Present() const
{
	// Copy framebuffer to texture
	void* pixels;
	int	  pitch;
	SDL_LockTexture(mTexture, nullptr, &pixels, &pitch);
	{
		AutoLockMutex lock(mFrameBufferMutex);
		for (int i = 0; i < 256; i++)
			memcpy((uint8*)pixels + i * pitch, mFrameBuffer + i * 256, 256 * sizeof(uint32));
	}
	SDL_UnlockTexture(mTexture);

	SDL_RenderTexture(mRenderer, mTexture, nullptr, nullptr);
	SDL_RenderPresent(mRenderer);
}

/**
 * @brief Process SDL events
 *
 * @param inEvent Event to be processed
 * @return SDL_AppResult Returns whether the application should continue
 */
SDL_AppResult NESApp::OnSDLEvent(const SDL_Event& inEvent)
{
	const SDL_EventType event_type = static_cast<SDL_EventType>(inEvent.type);

	// Handle the window close button
	if (event_type == SDL_EVENT_WINDOW_CLOSE_REQUESTED)
	{
		if (mWindow != nullptr && inEvent.window.windowID == SDL_GetWindowID(mWindow))
			return SDL_APP_SUCCESS;
	}

	// Handle keyboard events
	if (event_type == SDL_EVENT_KEY_UP)
	{
		if (inEvent.key.scancode == SDL_SCANCODE_K)
			mSaveStateRequest = ESaveStateRequest::Save;
		else if (inEvent.key.scancode == SDL_SCANCODE_L)
			mSaveStateRequest = ESaveStateRequest::Load;
	}
	else if (event_type == SDL_EVENT_GAMEPAD_ADDED)
	{
		AutoLockMutex lock(mPadMutex);

		// If no pad connected, connect to the pad just attached
		if (mPad == nullptr)
			mPad = SDL_OpenGamepad(inEvent.gdevice.which);
	}
	else if (event_type == SDL_EVENT_GAMEPAD_REMOVED)
	{
		AutoLockMutex lock(mPadMutex);
		if (mPad != nullptr && SDL_GetGamepadID(mPad) == inEvent.gdevice.which)
		{
			// Clear the old pad.
			mPad = nullptr;

			// Look for any pad that is not the one just removed.
			int				count;
			SDL_JoystickID* pads = SDL_GetGamepads(&count);

			for (int i = 0; i < count && mPad == nullptr; ++i)
				if (pads[i] != inEvent.gdevice.which)
					mPad = SDL_OpenGamepad(pads[i]);

			SDL_free(pads);
		}
	}

	return SDL_APP_CONTINUE;
}

/**
 * @brief Process audio data request
 *
 * Called on the audio device's own thread, right before it consumes data.
 * The emulator runs one frame AHEAD: the stream always holds >= 1 frame of
 * audio, so the callback can consume immediately - it never waits for the
 * emulator. The signal is a watermark: when less than a frame's worth is
 * left queued, tell the main thread to emulate the next frame. That keeps
 * supply locked to the sound card's consumption rate (master clock), with
 * ~1 frame of latency.
 *
 * @param inUserData
 * @param inStream
 * @param inAdditionalAmount
 * @param inTotalAmount
 */
void NESApp::sAudioDataRequested(void* inUserData, SDL_AudioStream* inStream, int inAdditionalAmount, int inTotalAmount)
{
	NESApp& app = *static_cast<NESApp*>(inUserData);

	// Get sample data
	uint8 sample_buffer[AudioBuffer::sMaxSize];
	int	  sample_count, buffer_size;
	{
		AutoLockMutex lock(app.mAudioBufferMutex);
		sample_count = app.mAudioBuffer.Pop(sample_buffer, inTotalAmount);
		buffer_size = app.mAudioBuffer.mSize;
	}

	// Check for buffer underflow
	if (sample_count < inAdditionalAmount)
		SDL_LogDebug(SDL_LOG_CATEGORY_AUDIO, "underflow (%d - %d - %d)\n", inTotalAmount, inAdditionalAmount,
					 sample_count);

	// Output samples
	SDL_PutAudioStreamData(inStream, sample_buffer, sample_count);

	// The audio device pulls the emulator: this callback fires on the
	// audio thread whenever the device needs more data, waking the emulator
	// when the buffer runs low.
	constexpr int kFrameAudioBytes = 736; // 44056Hz / 60.0988fps, mono S8
	while (buffer_size < 1.5 * kFrameAudioBytes)
	{
		SDL_SignalSemaphore(app.mFrameSem);
		buffer_size += kFrameAudioBytes;
	}
}

/**
 *
 * @param inAppState
 * @param argc
 * @param argv
 * @return
 */
SDL_AppResult SDL_AppInit(void** inAppState, int /*argc*/, char* /*argv*/[])
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
	*inAppState = app;
	return SDL_APP_CONTINUE;
}

/**
 *
 * @param inAppState
 * @param inEvent
 * @return
 */
SDL_AppResult SDL_AppEvent(void* inAppState, SDL_Event* inEvent)
{
	// Handle quit event
	if (inEvent->type == SDL_EVENT_QUIT)
		return SDL_APP_SUCCESS;

	return static_cast<NESApp*>(inAppState)->OnSDLEvent(*inEvent);
}

/**
 *
 * @param inAppState
 * @return
 */
SDL_AppResult SDL_AppIterate(void* inAppState)
{
	static_cast<NESApp*>(inAppState)->Present();
	return SDL_APP_CONTINUE;
}

/**
 *
 * @param inAppState
 * @param result
 */
void SDL_AppQuit(void* inAppState, SDL_AppResult /*inResult*/)
{
	if (auto* app = static_cast<NESApp*>(inAppState))
	{
		app->Exit();
		delete app;
	}

	SDL_Quit();
	SDL_Log("Application quit successfully!");
}
