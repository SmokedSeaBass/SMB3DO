#pragma once

#include <SDL.h>
#include <string>
#include "graphics.h"
#include "sprite.h"

class Tile {
public:
	unsigned char id_;  // Maybe change to string identifier?
	std::string name_;

	Tile(int id, Sprite* tileset, SDL_Rect sprite_rect, std::string name);
	~Tile();

	int Draw(Graphics& graphics, int pos_x, int pos_y);

private:
	Sprite* tileset_;
	SDL_Rect strip_rect_;
	SDL_Rect sprite_rect_;
	int frame_time_;
};