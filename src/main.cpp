// nes-emu - SDL application entry point

#include <SDL3/SDL_main.h>
#include "frontend/Frontend.h"

SDL_AppResult SDL_AppInit(void** inAppState, int /*argc*/, char* /*argv*/[])
{
	auto app = new Frontend();
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

SDL_AppResult SDL_AppEvent(void* inAppState, SDL_Event* inEvent)
{
	// Handle quit event
	if (inEvent->type == SDL_EVENT_QUIT)
		return SDL_APP_SUCCESS;

	return static_cast<Frontend*>(inAppState)->OnSDLEvent(*inEvent);
}

SDL_AppResult SDL_AppIterate(void* inAppState)
{
	static_cast<Frontend*>(inAppState)->Present();
	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* inAppState, SDL_AppResult /*inResult*/)
{
	if (auto* app = static_cast<Frontend*>(inAppState))
	{
		app->Exit();
		delete app;
	}

	SDL_Quit();
	SDL_Log("Application quit successfully!");
}
