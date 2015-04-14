	Introduction:
	The game is developed on OSX with Emacs, CMake and clang. The code base uses C++1y features and I have spent ~25 hours developing
	the game. This is the first game I ever developed (except for a Tetris that I wrote when in univerity for 26 years ago :-) )

	I started with the Grid class and implemented basic functions for generating a board, check for matches and similar functionality. I wanted
	to test as much of the functionality as possible before going grahic.

	Grid - generate, match, switch and collaps handling.
	midas_test - some basics tests for the functionality of the grid.

	When the basic functionality was working I started with the interactive parts of the game:

	MidasMiner - main gamle loop
	Board - acts on interaction and renders the game
	AssetManager - loads the graphic and the fonts
	Animation - base class and animations used by the game

	The game is driven by two methods exposed by Board: GetInteraction and Render.

	GetInteraction creates feedback in form of Animation that is input to the Render function.

	The animitions supported are:

	None - basically just moves the marker
	SwitchAnim - the user has switch jewels but there were no match
	MatchAnim - we have a match after a switch

	Requirement:
		sdl2 and sdl2_ttf. I installed them via brew
		The tests uses GTest the game will compile without GTest but you will get error when
		making the tests.

	Build:
		Create a new directory:

		cmake ..
		make

		All exectuables can be found here:
		midas/build/midas

	Folder Layout:
		cmake        - cmake find module for SDL
		assets       - graphics objects and fonts (stolen from osx)
		bin/darwin   - midas (the game) and midas_test compiled in release on OSX
		midas/src    - the source code for the game
		midas/test   - the source code for the test


	Regrets:
		I was to lazy to write press SPACE to play again after the game ends
		I really should have added more animations since it was fun and makes the game more fun to play.
		Hower being sick, having to work and celebrate my father in laws 80 birtday (both Saturday and Sunday),
		prevented such luxuaries.
		I do not support dragging.
		I do not ensure that newly added objects does not match during game (but I do it when a new board is generated)
		I do not ensure that a board has a solution - the probability is small but not zero
