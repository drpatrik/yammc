# yammc

![screenshots](screenshots/midas-demo-1.png)
![screenshots](screenshots/midas-demo-2.png)

        Yet Another Midas Miner Clone in C++

        My hobby project is to create a clone of the game Midas Miner. The
        artwork/fonts/sfx is not mine and I consider it to be public domain.
        If someone disagree please let me know and I will remove it.

        I have spent the last 30 years developing software but the last game
        I wrote was a Tetris clone for the Hercules graphic card back in 1989.

        My goal is to make the game as similar as possible to the original:
        http://www.royalgames.com/games/puzzle-games/midas-miner

        Next update might include anything of the following:

        - Windows port
        - Refactor game statistics into its own class
        - Introduce Z-order (again)

        The project requires cmake 2.8.8 or higher, SDL2, SDL2_ttf, SDL2_mixer and a C++14 compliant
        compiler.
        The test suit requires catch - Catch will be automatically download and installed
        as part of the build process

        Only tested under OSX. All my development machines are OSX and Linux these days.

        make

        will build the project, default is a release build. Ninja is used if available

        make test # will run the test suit.

        make run # will start the game

        Space restarts the game. M toggles the music on / off.

        Executables found here:

        ./build/midas/midas
        ./build/midas/midas_test

        Credits:
        All sfx and music are from www.freesound.org
        Fonts http://www.dafont.com/cabin.font
        Music by https://mrthenoronha.bandcamp.com
        FindSDL2_mixer https://github.com/rlsosborne/doom
        DeltaTimer adapted from http://headerphile.com/sdl2/sdl2-part-9-no-more-delays/
        https://github.com/philsquared/Catch
