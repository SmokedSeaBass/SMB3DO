#include "tile.hpp"

Tile::Tile(unsigned int id, Sprite* sprite, TileCollision collision) {
	id_ = id;
	if (sprite == nullptr) {
		sprite_ = nullptr;
	} else {
		AnimatedSprite* anim_sprite = dynamic_cast<AnimatedSprite*>(sprite);
		if (anim_sprite != nullptr) {
			sprite_ = anim_sprite;
		} else {
			sprite_ = sprite;
		}
	}
	top_collision_ = collision;
	side_collision_ = collision;
	bottom_collision_ = collision;
}

unsigned int Tile::GetId() const {
	return id_;
}

const Sprite* Tile::GetSprite() const {
	return sprite_;
}

Tile::TileCollision Tile::GetTopCollision() const {
	return top_collision_;
}

Tile::TileCollision Tile::GetSideCollision() const {
	return side_collision_;
}

Tile::TileCollision Tile::GetBottomCollision() const {
	return bottom_collision_;
}

void Tile::Update(double delta_time) {
	if (sprite_ == nullptr) {
		return;
	}
	sprite_->Update(delta_time);
}

int Tile::Draw(Graphics& graphics, int pos_x, int pos_y) const{
	if (sprite_ == nullptr) {
		//Error::PrintWarning("Attempted to draw tile #" + std::to_string(id_) + ", which has null sprite");
		return -1;
	}
	sprite_->Draw(graphics, {pos_x, pos_y});
}