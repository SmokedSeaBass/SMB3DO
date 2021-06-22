#include "tile.h"

#include "error.h"

Tile::Tile() : 
	id_(0),
	sprite_(nullptr),
	collision_(COLLISION_TYPE::NONE) {
}

Tile::Tile(unsigned int id, AnimatedSprite* sprite, COLLISION_TYPE collision) {
	id_ = id;
	sprite_ = sprite;
	collision_ = collision;
}

Tile::~Tile() {

}

unsigned int Tile::GetId() {
	return id_;
}

const AnimatedSprite* Tile::GetSprite() {
	return sprite_;
}

Tile::COLLISION_TYPE Tile::GetCollision() {
	return collision_;
}

void Tile::Update(int elapsed_time_ms) {
	if (sprite_ != nullptr) {
		sprite_->Update(elapsed_time_ms);
	}
}

int Tile::Draw(Graphics& graphics, int pos_x, int pos_y) {
	if (sprite_ == nullptr) {
		//Error::PrintWarning("Attempted to draw tile #" + std::to_string(id_) + ", which has null sprite");
		return -1;
	}
	return sprite_->Draw(graphics, pos_x, pos_y);
}