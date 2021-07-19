#include "tile.h"

#include "error.h"

Tile::Tile() : 
	id_(0),
	sprite_(nullptr),
	collision_(COLLISION_TYPE::NONE) {
}

Tile::Tile(unsigned int id, AnimatedSprite* sprite, COLLISION_TYPE collision) {
	id_ = id;
	if (sprite == nullptr) {
		sprite_ = nullptr;
	} else {
		sprite_ = std::make_shared<AnimatedSprite>(*sprite);
	}
	collision_ = collision;
}

unsigned int Tile::GetId() const {
	return id_;
}

const AnimatedSprite* Tile::GetSprite() const {
	return sprite_.get();
}

Tile::COLLISION_TYPE Tile::GetCollision() const {
	return collision_;
}

void Tile::Update(double delta_time) {
	if (sprite_ != nullptr) {
		sprite_->Update(delta_time);
	}
}

int Tile::Draw(Graphics& graphics, int pos_x, int pos_y) const{
	if (sprite_ == nullptr) {
		//Error::PrintWarning("Attempted to draw tile #" + std::to_string(id_) + ", which has null sprite");
		return -1;
	}
	return sprite_->Draw(graphics, pos_x, pos_y);
}