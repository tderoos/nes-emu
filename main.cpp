// nes-emu main

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "NES/NES.h"
#include "NES/types.h"



enum class ESaveStateRequest
{
	None, Load, Save
};


struct AppContext
{
    SDL_Semaphore*		mFrameSem = nullptr;
    int					mAudioDebt = 0;
    SDL_Window*			mWindow;
    SDL_Renderer*		mRenderer;
    SDL_Texture*		mTexture;
    SDL_AudioDeviceID	mAudioDevice;
    SDL_AudioStream*	mAudioStream;
    SDL_Joystick*		mStick;
    NES*				mNES = nullptr;
    bool			    mAppQuit = false;
	SaveState			mSaveState;
	ESaveStateRequest	mSaveStateRequest = ESaveStateRequest::None;
};

static void ROMSelected(void* userdata, const char* const* filelist, int filter)
{
    auto* app = (AppContext*)userdata;
    if (filelist && *filelist)
    {
        delete app->mNES;
        app->mNES = new NES(*filelist);
    }
    else
    {
        SDL_Log("No ROM selected, quitting.");
        app->mAppQuit = true;
    }
}

// Called on the audio device's own thread, when it needs more data.
// The callback cadence follows the device's buffer requests, which may
// arrive in bursts - not the NES frame cadence. So account the stream's
// audio deficit (input-format bytes needed) and signal the main thread
// once per NES-frame's worth of audio consumed (~736 samples, mono S8).
// Total supply always matches total demand, so emulation speed locks to
// real time; the accounting threshold only sets the steady-state latency.
static void AudioDataRequested(void* userdata, SDL_AudioStream* stream, int additional_amount, int total_amount)
{
    auto* app = (AppContext*)userdata;
    if (app->mNES == nullptr)
        return;

    constexpr int kFrameAudioBytes = 736;   // 44056Hz / 60.0988fps, mono S8
    app->mAudioDebt += additional_amount;
    while (app->mAudioDebt >= kFrameAudioBytes)
    {
        app->mAudioDebt -= kFrameAudioBytes;
        SDL_SignalSemaphore(app->mFrameSem);
    }
}

SDL_AppResult SDL_Fail()
{
    SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "Error %s", SDL_GetError());
    return SDL_APP_FAILURE;
}

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
    // init the library, here we make a window so we only need the Video capabilities.
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD | SDL_INIT_EVENTS))
        return SDL_Fail();

    // create a window
    SDL_Window* window = SDL_CreateWindow("NES", 512, 512, SDL_WINDOW_RESIZABLE);
    if (!window)
        return SDL_Fail();

    // vsync paces presentation to the display refresh
    SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer)
        return SDL_Fail();

    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_XRGB8888, SDL_TEXTUREACCESS_STREAMING, 256, 256);
    if (!texture)
        return SDL_Fail();

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


    int count;
    SDL_JoystickID* sticks = SDL_GetJoysticks(&count);
    SDL_Joystick* stick = nullptr;
    if (sticks && count > 0)
        stick = SDL_OpenJoystick(sticks[0]);
    SDL_free(sticks);

    // set up the application data
    *appstate = new AppContext {
       SDL_CreateSemaphore(0),
       0,
       window,
       renderer,
       texture,
       0,
       nullptr,
       stick,
       nullptr
    };

    // The audio device pulls the emulator: its get-callback fires on the
    // audio thread whenever the device needs more data, waking the main
    // thread to run exactly one frame. The sound card's clock is the master.
    SDL_AudioDeviceID audio_device = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &want);
    SDL_AudioStream* stream = SDL_CreateAudioStream(&src, &want);
    int res = SDL_BindAudioStream(audio_device, stream);
    ((AppContext*)*appstate)->mAudioDevice = audio_device;
    ((AppContext*)*appstate)->mAudioStream = stream;
    SDL_SetAudioStreamGetCallback(stream, AudioDataRequested, *appstate);
    SDL_ResumeAudioDevice(audio_device);

    // print some information about the window
    SDL_ShowWindow(window);
    {
        const char* rname = SDL_GetRendererName(renderer);
        SDL_Log("Renderer: %s", rname);

        int width, height, bbwidth, bbheight;
        SDL_GetWindowSize(window, &width, &height);
        SDL_GetWindowSizeInPixels(window, &bbwidth, &bbheight);
        SDL_Log("Window size: %ix%i", width, height);
        SDL_Log("Backbuffer size: %ix%i", bbwidth, bbheight);
        if (width != bbwidth)
        {
            SDL_Log("This is a highdpi environment.");
        }
    }

    // ask the user which ROM to load
    static const SDL_DialogFileFilter filters[] =
    {
        { "NES ROM", "nes" },
        { "All files", "*" }
    };
    SDL_ShowOpenFileDialog(ROMSelected, *appstate, window, filters, 2, nullptr, false);

    SDL_Log("Application started successfully!");

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    auto* app = (AppContext*)appstate;

	// Handle quit event
    if (event->type == SDL_EVENT_QUIT)
        app->mAppQuit = true;

    // Handle the window close button
    if (event->type == SDL_EVENT_WINDOW_CLOSE_REQUESTED
        && event->window.windowID == SDL_GetWindowID(app->mWindow))
        app->mAppQuit = true;

	// Handle keyboard events
	if (event->type == SDL_EVENT_KEY_UP)
	{
		if (event->key.scancode == SDL_SCANCODE_K)
			app->mSaveStateRequest = ESaveStateRequest::Save;
		else if(event->key.scancode == SDL_SCANCODE_L)
			app->mSaveStateRequest = ESaveStateRequest::Load;
	}

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate)
{
    auto* app = (AppContext*)appstate;

    // Handle quit request
    if (app->mAppQuit)
        return SDL_APP_SUCCESS;

    // Nothing to run until a ROM is selected
    if (app->mNES == nullptr)
        return SDL_APP_CONTINUE;

    // Wait for the audio device to request more data - it fires when the
    // stream runs low, pulling the emulator at the sound card's own rate.
    // No timers, no polling, no sleep-granularity stutter. The timeout
    // lets us notice quit requests even if the stream stays full (e.g.
    // after a long stall while the queue drains).
    while (!SDL_WaitSemaphoreTimeout(app->mFrameSem, 100) && !app->mAppQuit)
        ;

	// Handle load/save state request
	switch (app->mSaveStateRequest)
	{
	case ESaveStateRequest::None:
		break;

	case ESaveStateRequest::Load:
		app->mNES->ReadState(app->mSaveState);
		app->mSaveStateRequest = ESaveStateRequest::None;
		break;

	case ESaveStateRequest::Save:
		app->mNES->WriteState(app->mSaveState);
		app->mSaveStateRequest = ESaveStateRequest::None;
		break;
	}

    uint8 audio_buffer[768];
    uint32 audio_buffer_length = 0;
    memset(audio_buffer, 127, 768);

    uint8 controller_input = 0;
    if (app->mStick)
    {
        controller_input |= SDL_GetJoystickButton(app->mStick, 0) << 0;
        controller_input |= SDL_GetJoystickButton(app->mStick, 1) << 1;
        controller_input |= SDL_GetJoystickButton(app->mStick, 4) << 2;
        controller_input |= SDL_GetJoystickButton(app->mStick, 6) << 3;

        uint8 hat = SDL_GetJoystickHat(app->mStick, 0);
        controller_input |= (hat & 1) ? 1 << 4 : 0;
        controller_input |= (hat & 4) ? 1 << 5 : 0;
        controller_input |= (hat & 8) ? 1 << 6 : 0;
        controller_input |= (hat & 2) ? 1 << 7 : 0;
    }

    uint32 pixel_buffer[256*256];
    app->mNES->RunToVBlank(controller_input, pixel_buffer, audio_buffer, &audio_buffer_length);

    void* pixels;
    int pitch;
    SDL_LockTexture(app->mTexture, nullptr, &pixels, &pitch);
    for (int i = 0; i < 256; i++)
        memcpy((uint8*) pixels + i * pitch, pixel_buffer + i * 256, 256*sizeof(uint32));
    SDL_UnlockTexture(app->mTexture);

    SDL_RenderTexture(app->mRenderer, app->mTexture, nullptr, nullptr);
    SDL_RenderPresent(app->mRenderer);

    int res = SDL_PutAudioStreamData(app->mAudioStream, (void*)audio_buffer, audio_buffer_length);

    return SDL_APP_CONTINUE;
}



void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
    auto* app = (AppContext*)appstate;
    if (app)
    {
        SDL_DestroyRenderer(app->mRenderer);
        SDL_DestroyWindow(app->mWindow);
        delete app;
    }

    SDL_Quit();
    SDL_Log("Application quit successfully!");
}
