#pragma once
#include "graphics.h"

class Game {
public:
	Game();
	~Game();

	int Initialize();
	int Run();
	void Close();

private:
	Graphics graphics_;
};