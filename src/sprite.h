#pragma once

#include <SDL.h>
#include <string>
#include "graphics.h"

class Sprite {
public:
	Sprite(Graphics& graphics, const std::string& file_path, int source_x, int source_y, int width, int height);
	~Sprite();

	int Draw(Graphics& graphics, int pos_x, int pos_y);

private:
	SDL_Texture* texture_;
	SDL_Rect source_rect_;
};