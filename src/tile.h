#pragma once

#include <memory>
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

	Tile();
	Tile(unsigned int id, AnimatedSprite* sprite = nullptr, COLLISION_TYPE collision = COLLISION_TYPE::NONE);

	unsigned int GetId() const;
	const AnimatedSprite* GetSprite() const;
	COLLISION_TYPE GetCollision() const;

	void Update(double delta_time);
	int Draw(Graphics& graphics, int pos_x, int pos_y) const;

private:
	unsigned int id_;
	std::shared_ptr<AnimatedSprite> sprite_;
	COLLISION_TYPE collision_;
};