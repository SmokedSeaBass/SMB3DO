#pragma once
#include "SDL.h"
#include "graphics.h"

struct Game {
	Game();
	~Game();

	int init();
	int run();
	void close();

private:
	Graphics graphics;
};