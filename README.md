# yammc

![screenshots](screenshots/midas-demo-2.png)

        Yet Another Midas Miner Clone in C++
        ------------------------------------

        My hobby project is to create a clone of the game Midas Miner. The
        artwork/fonts/sfx is not mine and I consider it to be public domain.
        If someone disagree please let me know and I will remove it.

        I have spent the last 30 years developing software but the last game
        I wrote was a Tetris clone for the Hercules graphic card back in 1989.

        My goal is to make the game better then the original:
        http://www.royalgames.com/games/puzzle-games/midas-miner

        Features:
        - Smooth Animation
        - Music and sound
        - Advanced scoring system

        Space restarts the game. M toggles the music on / off.

        Dependencies:
        - C++14 compliant compiler (e.g. clang 3.4, clang-802, Visual Studio 2015 / 2017)
        - C++14 compliant STL implementation
        - cmake 2.8.8 or later
        - git
        - SDL2
        - SDL2_ttf
        - SDL2_mixer

        The test suit requires catch - Catch will be automatically downloaded and installed
        as part of the build process

        Tested under OSX and Windows 10. It should run under Linux and GCC as well

        OSX / Linux
        -----------

        Install the required libries with (linux)brew or any other package manager.

        make

        will build the project, default is a release build. Ninja is used if available

        make test # will run the test suit.

        make run # will start the game

        Windows 10
        ----------

        Set the following environment variables (see System Properties/Environemnt Variables...):

        SDL2DIR
        SDL2MIXER
        SDL2TTFDIR

        Example:
        SDL2DIR C:\SDL2-2.0.5

        The PATH should include all three libraries lib\x64 directories

        Example:
        PATH C:\SDL2-2.0.5\lib\x64;C:\SDL2_mixer\lib\x64;C:\SDL2_ttf-2.0.14\lib\x64

        Generate Visual Studio project files with CMakeSetup.exe

        Credits
        -------

        All sfx and music are from www.freesound.org
        Fonts http://www.dafont.com/cabin.font
        Music by https://mrthenoronha.bandcamp.com
        FindSDL2_mixer https://github.com/rlsosborne/doom
        DeltaTimer adapted from http://headerphile.com/sdl2/sdl2-part-9-no-more-delays/
        https://github.com/philsquared/Catch
