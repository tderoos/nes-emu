#include "Thread.h"

#include <cassert>

thread_local Thread* Thread::sCurrentThread;

bool Thread::sCreateThread(Thread& ioThread, ThreadFunc inFunc, const char* inName, void* inUserData)
{
	assert(ioThread.mThread == nullptr);

	struct sdl_thread_context
	{
		Thread*		mThread;
		ThreadFunc	mFunc;
		void*		mUserData;
	};

	auto sdl_thread_func = [](void* inContext)
	{
		auto ctx = static_cast<sdl_thread_context*>(inContext);
		sCurrentThread = ctx->mThread;
		const int val = ctx->mFunc(ctx->mUserData);
		sCurrentThread = nullptr;
		delete ctx;
		return val;
	};

	sdl_thread_context* thread_context = new sdl_thread_context(&ioThread, inFunc, inUserData);
	ioThread.mExit   = false;
	ioThread.mThread = SDL_CreateThread(sdl_thread_func, inName, thread_context);

	if (!ioThread.mThread)
		delete thread_context;

	return ioThread.mThread != nullptr;
}

void Thread::Destroy()
{
	if (!mThread)
		return;

	mExit = true;
	SDL_WaitThread(mThread, nullptr);
	mThread = nullptr;
}
