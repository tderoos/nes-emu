// WebFrontend - Emscripten + SDL3 web frontend
//
// Only the web-specific deltas live here; everything else is shared with
// the desktop Frontend:
//   - single-threaded main-loop emulation paced by a time accumulator
//   - ROM loading via the SDL file dialog (HTML picker) or ?rom=<url>
//   - .sav persistence through IDBFS mounted at /save (IndexedDB)

#include "WebFrontend.h"

#include <cstdlib>
#include <cstring>
#include <string>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/fetch.h>
#endif

// NTSC NES frame rate: 1789773 / 29780.5 ≈ 60.0988 fps
static constexpr double kFrameDurationSec = 1.0 / 60.0988;

// Never emulate more than this many frames in one iteration; after a longer
// stall (hidden tab) the backlog is dropped instead of spiraling.
static constexpr int kMaxCatchUpFrames = 5;

#ifdef __EMSCRIPTEN__

// Mount IDBFS at /save (NOT at '/': the root already has MEMFS mounted and
// remounting it throws "device busy"), then populate from IndexedDB.
static void sMountPersistentFS()
{
	EM_ASM(
		try { FS.mkdir('/save'); } catch (e) {}
		FS.mount(IDBFS, {}, '/save');
		FS.syncfs(true, function(err) { if (err) console.warn('nes-emu: IDBFS populate failed', err); });
	);
}

// Flush the VFS to IndexedDB (call after writing .sav files).
static void sSyncPersistentFS()
{
	EM_ASM(FS.syncfs(false, function(err) { if (err) console.warn('nes-emu: IDBFS sync failed', err); }));
}

// Read the ?rom=<url> query parameter, if present.
static std::string sGetQueryRomUrl()
{
	const char* ptr = reinterpret_cast<const char*>(EM_ASM_PTR({
		const v = new URLSearchParams(location.search).get('rom');
		if (!v)
			return 0;
		const bytes = new TextEncoder().encode(v);
		const p = _malloc(bytes.length + 1);
		HEAPU8.set(bytes, p);
		HEAPU8[p + bytes.length] = 0;
		return p;
	}));
	if (ptr == nullptr)
		return {};

	std::string url(ptr);
	free(const_cast<char*>(ptr));
	return url;
}

static void sFetchRomOnSuccess(emscripten_fetch_t* inFetch)
{
	auto* app			  = static_cast<WebFrontend*>(inFetch->userData);
	const std::string path = "/rom.nes";

	if (FILE* fd = fopen(path.c_str(), "wb"); fd != nullptr)
	{
		fwrite(inFetch->data, 1, inFetch->numBytes, fd);
		fclose(fd);
		// Defer to the main loop: starting emulation re-entrantly from this
		// XHR callback corrupts SDL's event-loop state.
		app->QueueRomLoad(path.c_str());
	}
	else
	{
		SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "Failed to write fetched ROM to VFS");
		app->RequestFileDialog();
	}

	emscripten_fetch_close(inFetch);
}

static void sFetchRomOnError(emscripten_fetch_t* inFetch)
{
	SDL_Log("ROM fetch failed, falling back to file dialog");
	static_cast<WebFrontend*>(inFetch->userData)->RequestFileDialog();
	emscripten_fetch_close(inFetch);
}

// Fetch a ROM from a (same-origin or CORS-enabled) URL into the VFS.
static void sLoadRomFromUrl(WebFrontend* inApp, const std::string& inUrl)
{
	emscripten_fetch_attr_t attr;
	emscripten_fetch_attr_init(&attr);
	strcpy(attr.requestMethod, "GET");
	attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
	attr.userData	= inApp;
	attr.onsuccess	= sFetchRomOnSuccess;
	attr.onerror	= sFetchRomOnError;
	emscripten_fetch(&attr, inUrl.c_str());
}

#endif // __EMSCRIPTEN__

#ifdef __EMSCRIPTEN__

static WebFrontend* sWebInstance = nullptr;

// Called from the page's HTML file input (shell.html) via Module.ccall:
// write the picked ROM into the VFS from JS, then load it here.
extern "C" EMSCRIPTEN_KEEPALIVE void nes_web_load_rom(const char* inPath)
{
	if (sWebInstance != nullptr)
		sWebInstance->QueueRomLoad(inPath);
}

// Copy a VFS file into the persistent /save mount, so the ROM and its
// <rom>.sav are backed by IndexedDB. Returns the new path.
static bool sCopyIntoPersistentDir(const char* inPath, std::string& outPath)
{
	const char* base = strrchr(inPath, '/');
	base = base != nullptr ? base + 1 : inPath;
	outPath = std::string("/save/") + base;

	FILE* in = fopen(inPath, "rb");
	if (in == nullptr)
		return false;
	FILE* out = fopen(outPath.c_str(), "wb");
	if (out == nullptr)
	{
		fclose(in);
		return false;
	}

	// Small stack buffer on purpose: the wasm stack is tiny (64KB default),
	// a large local here smashes it (stack cookie overwrite).
	uint8  buffer[4096];
	size_t count;
	while ((count = fread(buffer, 1, sizeof(buffer), in)) > 0)
		fwrite(buffer, 1, count, out);

	fclose(in);
	fclose(out);
	return true;
}

#endif // __EMSCRIPTEN__

/**
 * Web: load the ROM from the persistent /save mount (copied there first),
 * so its .sav file is persisted to IndexedDB.
 */
void WebFrontend::InsertRom(const char* inPath)
{
#ifdef __EMSCRIPTEN__
	std::string persistent_path;
	if (sCopyIntoPersistentDir(inPath, persistent_path))
		inPath = persistent_path.c_str();
#endif

	Frontend::InsertRom(inPath);
}

/**
 * Web startup: persistent FS first, then load a ROM from ?rom=<url> if
 * given, else ask the user via the file dialog.
 */
void WebFrontend::LoadInitialRom()
{
#ifdef __EMSCRIPTEN__
	sWebInstance = this;
	sMountPersistentFS();
	if (const std::string rom_url = sGetQueryRomUrl(); !rom_url.empty())
	{
		sLoadRomFromUrl(this, rom_url);
		return;
	}
#endif

	// No ?rom=: browsers only open a programmatic file picker from a user
	// gesture, so defer the dialog to the first input event.
	RequestFileDialog();
}

/**
 * Queue a ROM to be loaded on the next main-loop iteration.
 */
void WebFrontend::QueueRomLoad(const char* inPath)
{
	mPendingRomPath = inPath;
}

/**
 * Ask for the file dialog on the next user input event.
 */
void WebFrontend::RequestFileDialog()
{
	mWantFileDialog = true;
}

/**
 * @brief Defer the file dialog to the first user gesture, then pass the
 * event on to the base implementation.
 */
SDL_AppResult WebFrontend::OnSDLEvent(const SDL_Event& inEvent)
{
	if (mWantFileDialog &&
		(inEvent.type == SDL_EVENT_KEY_DOWN || inEvent.type == SDL_EVENT_MOUSE_BUTTON_DOWN))
	{
		mWantFileDialog = false;
		ShowFileDialog();
	}

	return Frontend::OnSDLEvent(inEvent);
}

/**
 * Web: no emulator thread - bind the audio stream and resume the device;
 * frames are produced from Iterate().
 */
void WebFrontend::StartEmulation()
{
	SDL_BindAudioStream(mAudioDevice, mAudioStream);
	SDL_ResumeAudioStreamDevice(mAudioStream);

	mFrameAccum	= 0.0;
	mLastTicksNS = 0;
}

/**
 *
 */
void WebFrontend::StopEmulation()
{
	SDL_PauseAudioStreamDevice(mAudioStream);
	SDL_UnbindAudioStream(mAudioStream);
	if (mAudioStream != nullptr)
		SDL_ClearAudioStream(mAudioStream);
}

/**
 * Web frame sink: push audio straight into the stream (single-threaded, no
 * ring buffer needed) and copy the framebuffer for presentation.
 */
void WebFrontend::OnFrameReady(const uint32* inPixelBuffer, const uint8* inAudioBuffer, uint32 inAudioLength)
{
	if (mAudioStream != nullptr)
		SDL_PutAudioStreamData(mAudioStream, inAudioBuffer, inAudioLength);

	memcpy(mFrameBuffer, inPixelBuffer, sizeof(FrameBuffer));
}

/**
 * Web: flush the .sav file to IndexedDB.
 */
void WebFrontend::OnSaveDataStored()
{
#ifdef __EMSCRIPTEN__
	sSyncPersistentFS();
#endif
}

/**
 * @brief Main-loop iteration (called once per animation frame on the web).
 * Emulates whole frames against a time accumulator so the emulation speed
 * tracks wall-clock time, then presents the latest frame.
 * @return
 */
SDL_AppResult WebFrontend::Iterate()
{
	const uint64 now = SDL_GetTicksNS();
	if (mLastTicksNS == 0)
		mLastTicksNS = now;
	const double dt = static_cast<double>(now - mLastTicksNS) / 1e9;
	mLastTicksNS	= now;

	// Load a queued ROM from the main loop (never from a fetch callback)
	if (!mPendingRomPath.empty())
	{
		const std::string path = mPendingRomPath;
		mPendingRomPath.clear();
		InsertRom(path.c_str());
	}

	if (mNES == nullptr)
		return SDL_APP_CONTINUE;

	mFrameAccum += dt;

	int frames = 0;
	while (mFrameAccum >= kFrameDurationSec && frames < kMaxCatchUpFrames)
	{
		RunFrame();
		ProcessSaveStateRequest();
		mFrameAccum -= kFrameDurationSec;
		++frames;
	}

	// After a stall (hidden tab) drop the backlog rather than catch up in a burst
	if (frames == kMaxCatchUpFrames)
		mFrameAccum = 0.0;

	if (frames > 0)
		Present();

	return SDL_APP_CONTINUE;
}
