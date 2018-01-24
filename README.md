# yammc - Yet Another Midas Miner Clone in C++

![screenshots](screenshots/midas-demo-2.png)

My hobby project is to create a clone of the game Midas Miner. The
artwork/fonts/sfx is not mine and I consider it to be public domain.
If someone disagree please let me know and I will remove it.

I have spent the last 30 years developing software but the last game
I wrote was a Tetris clone for the Hercules graphic card back in 1989.

My goal is to make the game better then the original:
http://www.royalgames.com/games/puzzle-games/midas-miner

**Features:**
* Smooth Animation
* Many effects
* Music and sound
* Advanced scoring system

**Space** restarts the game. **M** toggles the music on / off.

## Build YAMMC

**Dependencies:**
* C++17 compliant compiler (e.g. clang 4, clang-9.0.0, Visual Studio 2017.3 [P2])
* C++14 compliant STL implementation
* cmake 2.8.8 or later
* git
* SDL2 (x64 only)
* SDL2_ttf (x64 only)
* SDL2_mixer (x64 only)

The test suit requires catch - Catch will be automatically downloaded and installed
as part of the build process

Tested under OSX and x64 Windows 10. It should run under Linux and GCC as well

**OSX / Linux**

Install the required libraries with (linux)brew or any other package manager.

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
