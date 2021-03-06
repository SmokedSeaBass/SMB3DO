#pragma once
#include "sprite.h"

/// <summary>
/// A class that represents an "object" in NES terms. Can be a player, an enemy, score sprite, a projectiles, etc.
/// </summary>
class Entity {
public:
	Entity();
	~Entity();

	virtual void Update();
	virtual void Update(const Uint8* keyboard_state);
	virtual int Draw(Graphics& graphics);

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