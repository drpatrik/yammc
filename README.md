# yammc

        Yet Another Midas Miner Clone

        My hobby project is to create a Midas Miner Clone in C++11, the artwork/fonts
        is not mine and I consider it to be public domain. If someone disagree
        please let me know and I will remove it.

        I have spent the last 30 years developing software but the last game
        I wrote was a Tetris clone for the Hercules graphic card back in 1989.

        Still plenty of work. Next major update might include:

        - delta update instead of xx frame / s
        - Wiggle potential matches if player is idle
        - Animation when times up
        - Use Catch instead of gtest

        The project requires cmake 2.6 or higher, SDL2 and SDL2_ttf and a C++11 compliant compiler.
        The test suit requires gtest.

        Only tested under OSX

        make

        will build the project, default is a release build. Ninja is used if available

        Executables found here:

        ./build/midas/midas
        ./build/midas/midas_test

        or just type

        ./run
