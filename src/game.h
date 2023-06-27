#pragma once
#include "graphics.h"
#include "input.h"
#include "options.h"

/**
 *  \file game.h
 *
 *  Include file for the Game class
 */

class Game {
public:
	static double fps_limit;
	static float time_multiplier;

	static bool debug_show_hitboxes;
	static bool debug_show_info;

	Game();
	~Game();

	int GetInput(Input& input, SDL_Event& event, const Uint8* keyboard_state, bool& quit_game);
	int Update(const double delta_time);
	int Render(Graphics& graphics);

	/// <summary>
	/// Runs the game's main logical loop.
	/// </summary>
	/// <returns>0 on game loop end, -1 if the loop was terminated early</returns>
	int Run();

private:
	Options options_;
	Graphics graphics_;
};