#include "tile.h"

#include "error.h"

Tile::Tile() : 
	id_(0),
	sprite_(nullptr),
	collision_(COLLISION_TYPE::NONE) {
}

Tile::Tile(unsigned int id, Sprite* sprite, COLLISION_TYPE collision) {
	id_ = id;
	if (sprite == nullptr) {
		sprite_ = nullptr;
	} else {
		AnimatedSprite* anim_sprite = dynamic_cast<AnimatedSprite*>(sprite);
		if (anim_sprite != nullptr) {
			sprite_ = std::make_shared<AnimatedSprite>(*anim_sprite);
		} else {
			sprite_ = std::make_shared<Sprite>(*sprite);
		}
	}
	collision_ = collision;
}

unsigned int Tile::GetId() const {
	return id_;
}

const Sprite* Tile::GetSprite() const {
	return sprite_.get();
}

Tile::COLLISION_TYPE Tile::GetCollision() const {
	return collision_;
}

void Tile::Update(double delta_time) {
	if (sprite_ == nullptr) {
		return;
	}
	AnimatedSprite* anim_sprite = dynamic_cast<AnimatedSprite*>(sprite_.get());
	if (anim_sprite != nullptr) {
		return anim_sprite->Update(delta_time);
	} else {
		return sprite_->Update(delta_time);
	}
}

int Tile::Draw(Graphics& graphics, int pos_x, int pos_y) const{
	if (sprite_ == nullptr) {
		//Error::PrintWarning("Attempted to draw tile #" + std::to_string(id_) + ", which has null sprite");
		return -1;
	}
	AnimatedSprite* anim_sprite = dynamic_cast<AnimatedSprite*>(sprite_.get());
	if (anim_sprite != nullptr) {
		return anim_sprite->Draw(graphics, pos_x, pos_y);
	} else {
		return sprite_->Draw(graphics, pos_x, pos_y);
	}
}