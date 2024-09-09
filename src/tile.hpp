#pragma once

#include <memory>
#include <string>
#include "graphics.hpp"
#include "animated_sprite.hpp"

/// @brief Holds information about a tile from a tileset.
/// Usually generated from a .tsx file. Includes info such as
/// the tile's collision and animation.
class Tile {
public:
	enum class TileCollision {
		none,
		solid,
		coin,
		hurt,
		kill,
		hittable,
		bouncy,
		grabbable,
		water,
		ladder,
		slippery
	};

	Tile() = delete;
	Tile(unsigned int id, Sprite* sprite = nullptr, TileCollision collision = TileCollision::none);

	unsigned int GetId() const;
	const Sprite* GetSprite() const;
	TileCollision GetTopCollision() const;
	TileCollision GetSideCollision() const;
	TileCollision GetBottomCollision() const;

	void Update(double delta_time);
	int Draw(Graphics& graphics, int pos_x, int pos_y) const;

private:
	unsigned int id_;
	Sprite* sprite_;
	TileCollision top_collision_;
	TileCollision side_collision_;
	TileCollision bottom_collision_;
};