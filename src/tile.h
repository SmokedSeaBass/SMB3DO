#pragma once

#include <string>
#include "graphics.h"
#include "animated_sprite.h"

/// @brief A class that holds detailed information about a tile from a tileset.
///        Usually generated from a .tsx file and store in a tileset. Includes info such as
///        the tile's collision and animation.
class Tile {
public:
	enum class COLLISION_TYPE {
		NONE,
		SOLID
	};

	Tile(unsigned int id, AnimatedSprite* sprite, COLLISION_TYPE collision);
	~Tile();

	unsigned int GetId();
	COLLISION_TYPE GetCollision();

	void Update(int elapsed_time_ms);
	int Draw(Graphics& graphics, int pos_x, int pos_y);

private:
	unsigned int id_;
	AnimatedSprite* sprite_;
	COLLISION_TYPE collision_;
};