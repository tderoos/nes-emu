#pragma once

#include <SDL3/SDL_thread.h>

/**
 * Thread helper object
 */
class Thread
{
public:
			Thread() = default;
			~Thread() { Destroy(); }

	using ThreadFunc = int (*) (void *data);

	static bool	sCreateThread(Thread& ioThread, ThreadFunc inFunc, const char* inName, void* inUserData = nullptr);

	void	Destroy();

	bool	IsExit() const						{ return mExit; }
	static const Thread*	sGetCurrentThread()	{ return sCurrentThread; }

private:
	static thread_local	Thread*	sCurrentThread;
	bool						mExit   = false;
	SDL_Thread*					mThread = nullptr;
};
