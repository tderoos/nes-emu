#pragma once

#include <SDL3/SDL_mutex.h>

/**
 * Simple mutex wrapper around SDL_Mutex
 */
class Mutex
{
public:
			Mutex()			{ mMutex = SDL_CreateMutex(); }
			~Mutex()		{ SDL_DestroyMutex(mMutex); }

	void	Lock() const	{ SDL_LockMutex(mMutex); }
	void	Unlock() const	{ SDL_UnlockMutex(mMutex); }

private:
	SDL_Mutex*	mMutex = nullptr;
};

/**
 * Scoped lock, unlocks on scope exit
 */
struct AutoLockMutex
{
	AutoLockMutex(const Mutex& inMutex) : mMutex(inMutex)	{ mMutex.Lock(); }
	~AutoLockMutex()										{ mMutex.Unlock(); }

private:
	const Mutex&	mMutex;
};
