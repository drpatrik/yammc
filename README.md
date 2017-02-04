# yammc

        Yet Another Midas Miner Clone

        My hobby project is to create a Midas Miner Clone in C++11, the artwork/fonts
        is not mine and I consider it to be public domain. If someone disagree
        please let me know and I will remove it.

        I have spent the last 30 years developing software but the last game
        I wrote was a Tetris clone for the Hercules graphic card back in 1989.

        My goal is to make the game as similar as possible to the original:
        http://www.royalgames.com/games/puzzle-games/midas-miner

        Next update might include anything of the following:

        - Score animations when you clear more then 3 diamonds
        - Animation when you have been idle too long
        - Sound
        - Use Catch instead of gtest
        - Windows port

        The project requires cmake 2.6 or higher, SDL2 and SDL2_ttf and a C++11 compliant compiler.
        The test suit requires gtest - installation guide below. If you do not care about the test
        suit just delete enable_testing and everything below in ./midas/CMakeLists.txt

        Only tested under OSX. All my development machines are OSX and Linux these days.

        make

        will build the project, default is a release build. Ninja is used if available

        Executables found here:

        ./build/midas/midas
        ./build/midas/midas_test

        or just type

        ./run

        If someone want to do a Windows port I would appriticate a pull request. The same goes
        for any other modification. I am an utter novice in the land of game development.

        How to install gtest:

        cd gtest-1.7.0
        mkdir build
        cd build
        cmake ..
        make

        sudo cp libgtest.a /usr/local/lib
        sudo cp libgtest_main.a /usr/local/lib