#pragma once
#include "graphics.h"

/**
 *  \file game.h
 *
 *  Include file for the Game class
 */

class Game {
public:
	Game();
	~Game();

	/// <summary>
	/// Runs the game's main logical loop.
	/// </summary>
	/// <returns>0 on game loop end, -1 if the loop was terminated early</returns>
	int Run();
};