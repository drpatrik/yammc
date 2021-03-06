# YAMMC - Yet Another Midas Miner Clone

![screenshots](screenshots/midas-demo-2.png)

My hobby project is to create a clone of the game Midas Miner. The
artwork/fonts/sfx is not mine and I consider it to be public domain.
If someone disagree please let me know and I will remove it.

My goal is to make the game better then the original:
http://www.royalgames.com/games/puzzle-games/midas-miner

YAMMC has been tested and works under Windows 10 (x64 only), OSX 10.12/10.13/10.14/10.15/11.00, Raspian GNU/Linux 9/10 (stretch/buster) and Ubuntu 16.04
**Features:**
* Smooth Animation
* Many effects
* Music and sound
* Advanced scoring system

**Keyboard Commands**

Key | Action
--- | ------
Space  | Restart
M | Toggle music on/off
Trackpad / Mouse| Move cursor
Button 1|Select

## Build YAMMC

**Dependencies:**
* C++17 compliant compiler (e.g. clang 4 or newer, Visual Studio 2017.3 / 2019 or GCC 7.x.x)
* cmake 3.10.0 or later (Windows) or 3.0.0 or later (OSX/Linux)
* git
* SDL2 (x64 only)
* SDL2_ttf (x64 only)
* SDL2_mixer (x64 only)

The test suit requires catch - Catch will be automatically downloaded and installed
as part of the build process

**OSX / Linux**

Install the required libraries with apt/brew or any other package manager. Under Linux / Raspian install libsdl2-dev, libsdl2-ttf-dev and libsdl2-mixer-dev

Builds the project, default is a release build. Ninja is used if available:

```bash
make
```

Starts the game:
```bash
make run
```

Runs the test suit:

```bash
make test
```

Run cppcheck (if installed) on the codebase with all checks turned-on:

```bash
make cppcheck
```

**Raspian GNU/Linux**

You need to activate the OpenGL driver otherwise the game will be far to slow.

The code builds cleanly and has been tested with GCC 7.3 / 9.2 (with some warnings) and Clang 6 / 8 / 9.  instructions how
to install the compilers can be found here:

https://solarianprogrammer.com/2017/12/08/raspberry-pi-raspbian-install-gcc-compile-cpp-17-programs/
https://solarianprogrammer.com/2018/04/22/raspberry-pi-raspbian-install-clang-compile-cpp-17-programs/

You need to set these environment variables before running make:

```bash
export PATH=/usr/local/gcc-7.3.0/bin:$PATH
export CXX=gcc-7.3.0
```

or

```bash
export PATH=/usr/local/clang_6.0.0/bin:$PATH
export LD_LIBRARY_PATH=/usr/local/clang_6.0.0/lib:$LD_LIBRARY_PATH
export CXX=clang
```

If you install a newer version of gcc or clang ensure that you update PATH/LD_LIBRARY/CXX settings accordingly.

**64-bit Windows 10**

Set the following environment variables (see System Properties/Environment Variables...):

SDL2DIR
SDL2MIXER
SDL2TTFDIR

*Example:*
SDL2DIR C:\SDL2-2.0.5

The PATH should include all three libraries lib\x64 directories

*Example:*
PATH C:\SDL2-2.0.5\lib\x64;C:\SDL2_mixer\lib\x64;C:\SDL2_ttf-2.0.14\lib\x64

Generate Visual Studio project files with CMakeSetup.exe

## Credits

        All sfx and music are from www.freesound.org
        Fonts http://www.dafont.com/cabin.font
        Music by https://mrthenoronha.bandcamp.com
        FindSDL2_mixer https://github.com/rlsosborne/doom
        DeltaTimer adapted from http://headerphile.com/sdl2/sdl2-part-9-no-more-delays/
        https://github.com/philsquared/Catch
