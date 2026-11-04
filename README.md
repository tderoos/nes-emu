# nes-emu
NES emulator I wrote a long time ago. Written for fun, not performance or accuracy. 

## Building (desktop)

Requires CMake and a C++20 compiler. SDL3 is pulled in as a submodule:

```sh
git submodule update --init
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

## Building (web)

The web build uses Emscripten + SDL3 and is single-threaded, so it runs on
any static host (no COOP/COEP headers needed). Requires the [Emscripten SDK](https://emscripten.org/docs/getting_started/downloads.html):

```sh
git submodule update --init
emcmake cmake -B build-web -DCMAKE_BUILD_TYPE=Release
cmake --build build-web
```

This produces `build-web/Release/NES.html` (+ `.js` / `.wasm`). Serve the
build directory with any static file server, e.g.:

```sh
cd build-web/Release && python3 -m http.server
```

Open the page and pick a `.nes` ROM in the file dialog, or load one directly
from a same-origin (or CORS-enabled) URL:

```
http://localhost:8000/NES.html?rom=mario.nes
```

Controls: `WASD` = d-pad, `K` = A, `O` = B, `Space` = Select, `Return` =
Start, `[` = save state, `]` = load state. A gamepad works too. Battery
saves and save states persist in IndexedDB.
