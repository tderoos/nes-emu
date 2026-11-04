// nes-emu frontend

#include "Frontend.h"

#include <memory>

#include "common/SaveState.h"
#include "cores/nes/NES.h"

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
SDL_AppResult Frontend::Init()
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
void Frontend::Exit()
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
void Frontend::InsertRom(const char* inPath)
{
	if (mNES != nullptr)
		EjectRom();

	if (!mCartridge.Load(inPath))
		return;

	mNES = new NES(mCartridge);
	StartEmulation();
}

/**
 * Desktop: run the emulator on its own thread, paced by the audio device.
 */
void Frontend::StartEmulation()
{
	Thread::sCreateThread(
		mEmulatorThread,
		[](void* inAppState)
		{
			static_cast<Frontend*>(inAppState)->EmulatorThreadFunction();
			return 0;
		},
		"EmulatorThread", this);
}

/**
 *
 */
void Frontend::StopEmulation()
{
	// Exit the emulator thread
	mEmulatorThread.Destroy();
}

/**
 *
 */
void Frontend::EjectRom()
{
	if (mNES != nullptr)
	{
		StopEmulation();
		SafeDelete(mNES);

		// Save persistent game-state, including the active save state (if any)
		mCartridge.StoreSaveData();
		OnSaveDataStored();
	}

	mCartridge.Clear();
}

/**
 *
 * @return
 */
bool Frontend::ShowFileDialog()
{
	// ask the user which ROM to load
	static constexpr SDL_DialogFileFilter filters[] = {{"NES ROM", "nes"}, {"All files", "*"}};

	auto rom_selected = [](void* userdata, const char* const* filelist, int /*filter*/)
	{
		// filelist is NULL when the dialog is cancelled / unsupported
		if (filelist != nullptr && *filelist != nullptr)
			static_cast<Frontend*>(userdata)->InsertRom(*filelist);
	};

	SDL_ShowOpenFileDialog(rom_selected, this, mWindow, filters, 2, nullptr, false);
	return true;
}

/**
 * Desktop startup: ask the user which ROM to load.
 */
void Frontend::LoadInitialRom()
{
	ShowFileDialog();
}

/**
 * @brief Main-loop iteration: on desktop the emulator runs on its own
 * thread, so each iteration just presents the latest frame.
 * @return
 */
SDL_AppResult Frontend::Iterate()
{
	Present();
	return SDL_APP_CONTINUE;
}

/**
 * @brief Thread function to run the emulator
 *
 * This function runs the emulator, processing controller input and outputting audio and frame data.
 * It is paced by the audio-buffer. The requests for audio data provide a steady clock to sync the emulator
 * that is more accurate the vsync.
 */
void Frontend::EmulatorThreadFunction()
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
		ProcessSaveStateRequest();
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
 * @brief Handle a pending load/save state request, if any.
 */
void Frontend::ProcessSaveStateRequest()
{
	switch (mSaveStateRequest)
	{
	case ESaveStateRequest::Load:
	{
		LoadState load_state = mCartridge.GetLoadState();
		if (!load_state.IsEmpty())
			if (!mNES->ReadState(load_state))
				SDL_Log("Failed to load save state: incompatible or corrupt core state");
		break;
	}
	case ESaveStateRequest::Save:
	{
		SaveState save_state = mCartridge.GetSaveState();
		mNES->WriteState(save_state);
		break;
	}
	default:
		break;
	}
	mSaveStateRequest = ESaveStateRequest::None;
}

/**
 * @brief Flush battery state + snapshot to the .sav file immediately.
 */
void Frontend::FlushSaveData()
{
	if (mNES != nullptr)
	{
		mNES->StoreBatteryState();
		mCartridge.StoreSaveData();
		OnSaveDataStored();
	}
}

/**
 * @brief Get data from active controller
 * @return Bitfield containing button data
 */
uint8 Frontend::GetControllerData() const
{
	uint8		  controller_input = 0;

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
		controller_input |= key_state[SDL_SCANCODE_K] << 0;		 // A
		controller_input |= key_state[SDL_SCANCODE_O] << 1;		 // B
		controller_input |= key_state[SDL_SCANCODE_SPACE] << 2;	 // Select
		controller_input |= key_state[SDL_SCANCODE_RETURN] << 3; // Start

		controller_input |= key_state[SDL_SCANCODE_W] << 4; // Up
		controller_input |= key_state[SDL_SCANCODE_S] << 5; // Down
		controller_input |= key_state[SDL_SCANCODE_A] << 6; // Left
		controller_input |= key_state[SDL_SCANCODE_D] << 7; // Right
	}

	return controller_input;
}

/**
 * @brief Run a single frame, output generated data to frame and audio buffer.
 */
void Frontend::RunFrame()
{
	uint8 audio_buffer[768];
	uint32 audio_buffer_length = 0;

	// Heap-allocate the 256KB frame buffer: a stack local this size
	// overflows the (small) wasm stack on web builds. Note: make_unique
	// needs the unbounded array form - make_unique<FrameBuffer>() is
	// ill-formed because FrameBuffer is a bounded C array type.
	auto pixel_buffer = std::make_unique<uint32[]>(256 * 256);
	mNES->RunToVBlank(GetControllerData(), pixel_buffer.get(), audio_buffer, &audio_buffer_length);

	OnFrameReady(pixel_buffer.get(), audio_buffer, audio_buffer_length);
}

/**
 * Desktop frame sink: queue audio for the audio callback and copy the
 * framebuffer for presentation (both under mutex; the emulator thread
 * produces while the main thread presents).
 */
void Frontend::OnFrameReady(const uint32* inPixelBuffer, const uint8* inAudioBuffer, uint32 inAudioLength)
{
	{
		AutoLockMutex lock(mAudioBufferMutex);
		mAudioBuffer.Push(inAudioBuffer, inAudioLength);
	}
	{
		AutoLockMutex lock(mFrameBufferMutex);
		memcpy(mFrameBuffer, inPixelBuffer, sizeof(FrameBuffer));
	}
}

/**
 * @brief Present the latest framebuffer
 */
void Frontend::Present() const
{
	// Copy framebuffer to texture
	void* pixels;
	int	  pitch;
	SDL_LockTexture(mTexture, nullptr, &pixels, &pitch);
	{
		AutoLockMutex lock(mFrameBufferMutex);
		for (int i = 0; i < 256; i++)
			memcpy(static_cast<uint8*>(pixels) + i * pitch, mFrameBuffer + i * 256, 256 * sizeof(uint32));
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
SDL_AppResult Frontend::OnSDLEvent(const SDL_Event& inEvent)
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
		if (inEvent.key.scancode == SDL_SCANCODE_LEFTBRACKET)
			mSaveStateRequest = ESaveStateRequest::Save;
		else if (inEvent.key.scancode == SDL_SCANCODE_RIGHTBRACKET)
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
void Frontend::sAudioDataRequested(void* inUserData, SDL_AudioStream* inStream, int inAdditionalAmount, int inTotalAmount)
{
	Frontend& app = *static_cast<Frontend*>(inUserData);

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
