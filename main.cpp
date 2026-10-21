// nes-emu main

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "nestor/nestor.h"
#include "nestor/types.h"



enum class ESaveStateRequest
{
	None, Load, Save
};


struct AppContext {
    SDL_Window*			window;
    SDL_Renderer*		renderer;
    SDL_Texture*		texture;
    SDL_AudioDeviceID	audio_device;
    SDL_AudioStream*	audio_stream;
    SDL_Joystick*		stick;
    nestor*				mNestor = nullptr;
    bool			    app_quit = false;
	SaveState			mSaveState;
	ESaveStateRequest	mSaveStateRequest = ESaveStateRequest::None;
};

SDL_AppResult SDL_Fail() {
    SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "Error %s", SDL_GetError());
    return SDL_APP_FAILURE;
}

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {
    // init the library, here we make a window so we only need the Video capabilities.
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD | SDL_INIT_EVENTS)) {
        return SDL_Fail();
    }

    // create a window
    SDL_Window* window = SDL_CreateWindow("Window", 512, 512, SDL_WINDOW_RESIZABLE);
    if (!window) {
        return SDL_Fail();
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        return SDL_Fail();
    }

    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 256, 256);
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


    SDL_AudioDeviceID audio_device = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &want);

    SDL_AudioStream* stream = SDL_CreateAudioStream(&src, &want);
    int res = SDL_BindAudioStream(audio_device, stream);
    SDL_ResumeAudioDevice(audio_device);

    int count;
    SDL_JoystickID* sticks = SDL_GetJoysticks(&count);
    SDL_Joystick* stick = SDL_OpenJoystick(sticks[0]);

    // print some information about the window
    SDL_ShowWindow(window);
    {
        int width, height, bbwidth, bbheight;
        SDL_GetWindowSize(window, &width, &height);
        SDL_GetWindowSizeInPixels(window, &bbwidth, &bbheight);
        SDL_Log("Window size: %ix%i", width, height);
        SDL_Log("Backbuffer size: %ix%i", bbwidth, bbheight);
        if (width != bbwidth) {
            SDL_Log("This is a highdpi environment.");
        }
    }

    // set up the application data
    *appstate = new AppContext{
       window,
       renderer,
       texture,
       audio_device,
       stream,
       stick,
       new nestor("C:\\Users\\Tommy de Roos\\Projects\\nes-emu\\Roms\\smb1.nes")
    };

    SDL_Log("Application started successfully!");

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    auto* app = (AppContext*)appstate;

	// Handle quit event	
    if (event->type == SDL_EVENT_QUIT) {
        app->app_quit = true;
    }

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

SDL_AppResult SDL_AppIterate(void* appstate) {
    auto* app = (AppContext*)appstate;

	// Handle load/save state request
	switch (app->mSaveStateRequest)
	{
	case ESaveStateRequest::None:
		break;

	case ESaveStateRequest::Load:
		app->mNestor->ReadState(app->mSaveState);
		app->mSaveStateRequest = ESaveStateRequest::None;
		break;

	case ESaveStateRequest::Save:
		app->mNestor->WriteState(app->mSaveState);
		app->mSaveStateRequest = ESaveStateRequest::None;
		break;
	}

    void* pixels;
    int pitch;
    SDL_LockTexture(app->texture, nullptr, &pixels, &pitch);

    uint8 audio_buffer[768];
    uint32 audio_buffer_length = 0;
    memset(audio_buffer, 127, 768);

    uint8 controller_input = 0;
    controller_input |= SDL_GetJoystickButton(app->stick, 0) << 0;
    controller_input |= SDL_GetJoystickButton(app->stick, 1) << 1;
    controller_input |= SDL_GetJoystickButton(app->stick, 4) << 2;
    controller_input |= SDL_GetJoystickButton(app->stick, 6) << 3;

    uint8 hat = SDL_GetJoystickHat(app->stick, 0);
    controller_input |= (hat & 1) ? 1 << 4 : 0;
    controller_input |= (hat & 4) ? 1 << 5 : 0;
    controller_input |= (hat & 8) ? 1 << 6 : 0;
    controller_input |= (hat & 2) ? 1 << 7 : 0;

    app->mNestor->RunToVBlank(controller_input, (uint32*)pixels, audio_buffer, &audio_buffer_length);
    SDL_UnlockTexture(app->texture);
    SDL_RenderTexture(app->renderer, app->texture, nullptr, nullptr);
    SDL_RenderPresent(app->renderer);

    int res = SDL_PutAudioStreamData(app->audio_stream, (void*)audio_buffer, audio_buffer_length);
    SDL_FlushAudioStream(app->audio_stream);

    return SDL_APP_CONTINUE;
}



void SDL_AppQuit(void* appstate, SDL_AppResult result) {
    auto* app = (AppContext*)appstate;
    if (app) {
        SDL_DestroyRenderer(app->renderer);
        SDL_DestroyWindow(app->window);
        delete app;
    }

    SDL_Quit();
    SDL_Log("Application quit successfully!");
}
