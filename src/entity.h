#pragma once

#include "input.h"
#include "sprite.h"
#include "tilemap.h"

/// <summary>
/// A class that represents an "object" in NES terms. Can be a player, an enemy, score sprite, a projectiles, etc.
/// </summary>
class Entity {
public:
	Entity();
	~Entity();

	std::vector<double> GetPosition() const;
	std::vector<double> GetVelocity() const;

	virtual void Update(const Input& input, double delta_time, const Tilemap& tilemap);
	virtual int Draw(Graphics& graphics, double offset_x = 0, double offset_y = 0);

protected:
	Sprite* sprite_;
	double pos_x_;
	double pos_y_;
	double vel_x_;
	double vel_y_;
	/// <summary>
	/// The box that defines the entity's hitbox relative to its (x, y) position.  If either width or height is negative,
	/// the collider is treated as non-existant.
	/// </summary>
	SDL_Rect collider_;
};