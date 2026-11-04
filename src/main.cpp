// nes-emu - SDL application entry point
//
// Instantiates the platform frontend: the threaded desktop Frontend or,
// under Emscripten, the single-threaded WebFrontend.

#include <SDL3/SDL_main.h>

#ifdef __EMSCRIPTEN__
#include "frontend/WebFrontend.h"
using AppFrontend = WebFrontend;
#else
#include "frontend/Frontend.h"
using AppFrontend = Frontend;
#endif

SDL_AppResult SDL_AppInit(void** inAppState, int /*argc*/, char* /*argv*/[])
{
	auto app = new AppFrontend();
	if (SDL_AppResult result = app->Init(); result != SDL_APP_CONTINUE)
	{
		delete app;
		return result;
	}

	// Load the initial ROM (platform-specific: file dialog or ?rom=<url>)
	app->LoadInitialRom();

	// Set the app the the SDL app state and continue execution
	*inAppState = app;
	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* inAppState, SDL_Event* inEvent)
{
	// Handle quit event
	if (inEvent->type == SDL_EVENT_QUIT)
		return SDL_APP_SUCCESS;

	return static_cast<AppFrontend*>(inAppState)->OnSDLEvent(*inEvent);
}

SDL_AppResult SDL_AppIterate(void* inAppState)
{
	return static_cast<AppFrontend*>(inAppState)->Iterate();
}

void SDL_AppQuit(void* inAppState, SDL_AppResult /*inResult*/)
{
	if (auto* app = static_cast<AppFrontend*>(inAppState))
	{
		app->Exit();
		delete app;
	}

	SDL_Quit();
	SDL_Log("Application quit successfully!");
}
