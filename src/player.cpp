#include "player.h"

#include <assert.h>
#include "constants.h"
#include "error.h"
#include "game.h"	 

const Rectangle COLLIDER_TOP(-1, -13, 2, 1);		// Special collider that's skinnier and causes easier sideways "sliding"
const Rectangle COLLIDER_BOTTOM(-4, -2, 8, 1);
const Rectangle COLLIDER_LEFT(-5, -12, 1, 8);
const Rectangle COLLIDER_RIGHT(4, -12, 1, 8);

Player::Player() : 
	dpad_vector_({ 0, 0 }),
	dir_facing_(1),
	speed_grounded_(0.0),
	aerial_speed_cap_(Physics::MAX_SPEED_RUN),
	status_(0) {
	collider_ = { -5, -13, 10, 9 };  // 0-2 [3-12] 13-15 on x, 0-1 [2-14] 15 on y for small mario
}

Player::Player(Graphics& graphics, const std::string& path_to_bmp, double pos_x, double pos_y) : Player::Player() {
	pos_x_ = pos_x;
	pos_y_ = pos_y;
	// Initialize spritemap
	// All hardcoded for now
	// Walk animation speeds are 8 (crawl), 12 (walk), 16 (run)
	SDL_Texture* player_texture = graphics.LoadTextureFromImage(path_to_bmp, 0, 0);

	Sprite spr_stand = Sprite(graphics, player_texture, 17 + 1, 1, 16, 16);
	spr_stand.SetOrigin(Sprite::ORIGIN_ORIENTATION::BOTTOM_MIDDLE);
	sprite_map_["stand"] = std::make_unique<Sprite>(spr_stand);

	AnimatedSprite spr_walk = AnimatedSprite(graphics, player_texture, 1, 1, 16, 16, 12, 2, 1);
	spr_walk.SetOrigin(Sprite::ORIGIN_ORIENTATION::BOTTOM_MIDDLE);
	sprite_map_["walk"] = std::make_unique<AnimatedSprite>(spr_walk);

	Sprite spr_jump = Sprite(graphics, player_texture, 17*2 + 1, 1, 16, 16);
	spr_jump.SetOrigin(Sprite::ORIGIN_ORIENTATION::BOTTOM_MIDDLE);
	sprite_map_["jump"] = std::make_unique<Sprite>(spr_jump);

	Sprite spr_skid = Sprite(graphics, player_texture, 86, 1, 16, 16);
	spr_skid.SetOrigin(Sprite::ORIGIN_ORIENTATION::BOTTOM_MIDDLE);
	sprite_map_["skid"] = std::make_unique<Sprite>(spr_skid);

	sprite_ = sprite_map_["stand"].get();
}

Player::Player(Sprite* sprite, double pos_x, double pos_y) : Player::Player() {
	sprite_ = sprite;
	pos_x_ = pos_x;
	pos_y_ = pos_y;
}

Player::~Player() { }

void Player::HandleInputs(const Input& input) {
	dpad_vector_ = { 0, 0 };
	if (input.IsButtonDown(Input::Button::P1_RIGHT))
		dpad_vector_[0] += 1;
	if (input.IsButtonDown(Input::Button::P1_UP))
		dpad_vector_[1] += -1;
	if (input.IsButtonDown(Input::Button::P1_LEFT))
		dpad_vector_[0] += -1;
	if (input.IsButtonDown(Input::Button::P1_DOWN))
		dpad_vector_[1] += 1;
	
	if (dpad_vector_[0] != 0)
		dir_facing_ = dpad_vector_[0];
}

void Player::Update(const Input& input, double delta_time, const Tilemap& tilemap) {
	/* Get input */
	HandleInputs(input);

	/* Apply physics to get intended position */
	double dt_ratio = delta_time / (1000.0 / 60.0);

	// Jumping
	if ((status_ & (unsigned char)Status::GROUNDED) && (input.IsButtonPressed(Input::Button::P1_A))) {
		if (abs(vel_x_) > (double)0x0300 * dt_ratio / 256.0) vel_y_ = -Physics::JUMP_VEL_SPRINT * dt_ratio;
		else if (abs(vel_x_) > (double)0x0200 * dt_ratio / 256.0) vel_y_ = -Physics::JUMP_VEL_RUN * dt_ratio;
		else if (abs(vel_x_) > (double)0x0100 * dt_ratio / 256.0) vel_y_ = -Physics::JUMP_VEL_WALK * dt_ratio;
		else vel_y_ = -Physics::JUMP_VEL_STAND * dt_ratio;
		status_ &= ~(unsigned char)Status::GROUNDED;
	}

	// Lateral movement
	if (status_ & (unsigned char)Status::GROUNDED) {								// Grounded physics
		if (dpad_vector_[0] == 0) {												// Neutral input, normal deceleration
			if (abs(vel_x_) < Physics::DECEL_GROUND_NORMAL * dt_ratio * dt_ratio) vel_x_ = 0;
			else vel_x_ -= Physics::DECEL_GROUND_NORMAL * dt_ratio * dt_ratio * sgn(vel_x_);
		} else if (dpad_vector_[0] == -sgn(vel_x_)) {								// Opposing input, skid deceleration
			vel_x_ += dpad_vector_[0] * Physics::SKID_GROUND_NORMAL * dt_ratio * dt_ratio;
		} else {																	// Accelerating
			vel_x_ += dpad_vector_[0] * Physics::ACCEL_GROUND * dt_ratio * dt_ratio;
			double ground_speed_cap = Physics::MAX_SPEED_WALK * dt_ratio;
			if (input.IsButtonDown(Input::Button::P1_B)) ground_speed_cap = Physics::MAX_SPEED_RUN * dt_ratio;
			if (abs(vel_x_) > ground_speed_cap) {									// Ground x-speed limit
				vel_x_ = ground_speed_cap * sgn(vel_x_);							// TODO: Implement gradual slowing down if cap is exceeded
			}
		}
		// Set air speed cap based on acheived ground speed
		//if (abs(vel_x_) <= Physics::MAX_SPEED_WALK * dt_ratio) aerial_speed_cap_ = Physics::MAX_SPEED_WALK * dt_ratio;
		//else if (abs(vel_x_) >= Physics::MAX_SPEED_SPRINT * dt_ratio) aerial_speed_cap_ = Physics::MAX_SPEED_SPRINT * dt_ratio;
		//else aerial_speed_cap_ = Physics::MAX_SPEED_RUN * dt_ratio;
	} else {																	// Mid-air physics
		if (input.IsButtonDown(Input::Button::P1_B)) {
			aerial_speed_cap_ = Physics::MAX_SPEED_RUN * dt_ratio;
		} else {
			aerial_speed_cap_ = Physics::MAX_SPEED_WALK * dt_ratio;
		}
		if (dpad_vector_[0] == -sgn(vel_x_)) {										// Mid-air skidding
			vel_x_ += dpad_vector_[0] * Physics::SKID_AIR * dt_ratio * dt_ratio;
		} else {																	// Mid-air accelerating/neutral
			double accel = dpad_vector_[0] * Physics::ACCEL_AIR * dt_ratio * dt_ratio;
			if (abs(vel_x_ + accel) > aerial_speed_cap_) {							// Only speed up if aerial speed cap allows.  Otherwise, keep current speed
				accel = 0;
			}
			vel_x_ += accel;
		}
		//if (abs(vel_x_) > aerial_speed_cap_) {
		//	vel_x_ -= Physics::SKID_AIR * dt_ratio * sgn(vel_x_);								// Mid-air x-speed limit (forced deceleration)
		//}
	}

	// Gravity
	if (input.IsButtonDown(Input::Button::P1_A) && (vel_y_ < (double)-0x0200 * dt_ratio / 256.0)) {
		vel_y_ += Physics::GRAVITY_LIGHT * dt_ratio * dt_ratio;
	} else {
		vel_y_ += Physics::GRAVITY_HEAVY * dt_ratio * dt_ratio;
	}
	if (vel_y_ > Physics::VEL_TERM * dt_ratio) vel_y_ = Physics::VEL_TERM * dt_ratio;

	/* Check solid collisions and make corrections */
	double delta_x = vel_x_;
	double delta_y = vel_y_;
	if (delta_x >= 0) {
		// React to collision RIGHT
		CollisionInfo info = GetCollisionInfo(tilemap, RightCollision(delta_x));
		if (info.collided) {
			//pos_x_ = info.col * TILESIZE_NES - COLLIDER_X.Right();
			pos_x_ = std::max(info.col * TILESIZE_NES - COLLIDER_RIGHT.Right(), pos_x_ - 1 * dt_ratio);
			vel_x_ = 0;
		} else {
			pos_x_ += delta_x;
		}
		// Check for collision in opposite direction
		info = GetCollisionInfo(tilemap, LeftCollision(0));
		if (info.collided) {
			pos_x_ = std::min((info.col + 1) * TILESIZE_NES - COLLIDER_LEFT.Left(), pos_x_ + 1 * dt_ratio);
		}
	} else {
		// React to collision LEFT
		CollisionInfo info = GetCollisionInfo(tilemap, LeftCollision(delta_x));
		if (info.collided) {
			//pos_x_ = (info.col + 1) * TILESIZE_NES - COLLIDER_X.Left();
			pos_x_ = std::min((info.col + 1) * TILESIZE_NES - COLLIDER_LEFT.Left(), pos_x_ + 1 * dt_ratio);
			vel_x_ = 0;
		} else {
			pos_x_ += delta_x;
		}
		// Check for collision in opposite direction
		info = GetCollisionInfo(tilemap, RightCollision(0));
		if (info.collided) {
			pos_x_ = std::max(info.col * TILESIZE_NES - COLLIDER_RIGHT.Right(), pos_x_ - 1 * dt_ratio);
		}
	}
	if (delta_y > 0) {
		// React to collision DOWN
		CollisionInfo info = GetCollisionInfo(tilemap, BottomCollision(delta_y));
		if (info.collided) {
			pos_y_ = info.row * TILESIZE_NES - COLLIDER_BOTTOM.Bottom();
			vel_y_ = 0;
			status_ |= (unsigned char)Status::GROUNDED;
		} else {
			pos_y_ += delta_y;
			status_ &= ~(unsigned char)Status::GROUNDED;
		}
		// Check for collision in opposite direction
		info = GetCollisionInfo(tilemap, TopCollision(0));
		if (info.collided) {
			pos_y_ = (info.row + 1) * TILESIZE_NES - COLLIDER_TOP.Top();
		}
	} else {
		// React to collision UP
		CollisionInfo info = GetCollisionInfo(tilemap, TopCollision(delta_y));
		if (info.collided) {
			pos_y_ = (info.row + 1) * TILESIZE_NES - COLLIDER_TOP.Top();
			vel_y_ = 0;
		} else {
			pos_y_ += delta_y;
		}
		// Check for collision in opposite direction
		info = GetCollisionInfo(tilemap, BottomCollision(0));
		if (info.collided) {
			pos_y_ = info.row * TILESIZE_NES - COLLIDER_BOTTOM.Bottom();
			status_ |= (unsigned char)Status::GROUNDED;
		}
	}

	// Update Sprite
	Sprite* new_sprite = nullptr;
	double speed_x = abs(vel_x_);
	if (status_ & (unsigned char)Status::GROUNDED) {
		if (speed_x == 0) {
			new_sprite = sprite_map_["stand"].get();
		} else if (speed_x < Player::Physics::MAX_SPEED_WALK * dt_ratio) {
			new_sprite = sprite_map_["walk"].get();
			new_sprite->SetAnimationSpeed(6 * (1000.0 / 60.0));
		} else if (speed_x < Player::Physics::MAX_SPEED_RUN * dt_ratio) {
			new_sprite = sprite_map_["walk"].get();
			new_sprite->SetAnimationSpeed(4 * (1000.0 / 60.0));
		} else {
			new_sprite = sprite_map_["walk"].get();
			new_sprite->SetAnimationSpeed(2 * (1000.0 / 60.0));
		}
		if (dpad_vector_[0] != 0 && sgn(vel_x_) == -dpad_vector_[0]) {
			new_sprite = sprite_map_["skid"].get();
		}
	} else {
		new_sprite = sprite_map_["jump"].get();
	}
	if (new_sprite != sprite_) {
		sprite_ = new_sprite;
		sprite_->ResetAnimation();
	}
	if (sprite_ != nullptr) {
		sprite_->Update(delta_time);
	}
}

int Player::Draw(Graphics& graphics, double offset_x, double offset_y) {
	SDL_RendererFlip flip = SDL_FLIP_NONE;
	if (dir_facing_ == -1) {
		flip = SDL_FLIP_HORIZONTAL;
	}
	int return_code = sprite_->Draw(graphics, static_cast<int>(floor(pos_x_ + offset_x)) , static_cast<int>(floor(pos_y_ + offset_y)), flip);
	if (Game::debug_show_hitboxes) {
		graphics.DrawColoredRect({ floor(pos_x_) - 8 + offset_x, floor(pos_y_) - 16 + offset_y, 16, 16 }, 0x00, 0x00, 0x00, 0x40);
		graphics.DrawColoredRect(RightCollision(0) + Rectangle(offset_x, offset_y, 0, 0 ), 0xFF, 0xFF, 0xFF, 0xFF);
		graphics.DrawColoredRect(LeftCollision(0) + Rectangle(offset_x, offset_y, 0, 0), 0xFF, 0xFF, 0xFF, 0xFF);
		graphics.DrawColoredRect(BottomCollision(0) + Rectangle(offset_x, offset_y, 0, 0), 0xFF, 0xFF, 0xFF, 0xFF);
		graphics.DrawColoredRect(TopCollision(0) + Rectangle(offset_x, offset_y, 0, 0), 0xFF, 0xFF, 0xFF, 0xFF);
	}
	return return_code;
}

Rectangle Player::GetColliderAbsoluteRect() {
	Rectangle abs_rect = {
		(double)collider_.x + pos_x_,
		(double)collider_.y + pos_y_,
		(double)collider_.w,
		(double)collider_.h
	};
	return abs_rect;
}

Rectangle Player::LeftCollision(double delta) const {
	assert(delta <= 0);
	Rectangle delta_rect(
		floor(pos_x_) + COLLIDER_LEFT.Left() + delta,
		floor(pos_y_) + COLLIDER_LEFT.Top(),
		COLLIDER_LEFT.w - delta,
		COLLIDER_LEFT.h
	);
	return delta_rect;
}

Rectangle Player::RightCollision(double delta) const {
	assert(delta >= 0);
	Rectangle delta_rect(
		floor(pos_x_) + COLLIDER_RIGHT.Left(),
		floor(pos_y_) + COLLIDER_RIGHT.Top(),
		COLLIDER_RIGHT.w + delta,
		COLLIDER_RIGHT.h
	);
	return delta_rect;
}

Rectangle Player::TopCollision(double delta) const {
	assert(delta <= 0);
	Rectangle delta_rect(
		floor(pos_x_) + COLLIDER_TOP.Left(),
		floor(pos_y_) + COLLIDER_TOP.Top() + delta,
		COLLIDER_TOP.w,
		COLLIDER_TOP.h - delta
	);
	return delta_rect;
}

Rectangle Player::BottomCollision(double delta) const {
	assert(delta >= 0);
	Rectangle delta_rect(
		floor(pos_x_) + COLLIDER_BOTTOM.Left(),
		floor(pos_y_) + COLLIDER_BOTTOM.Top(),
		COLLIDER_BOTTOM.w,
		COLLIDER_BOTTOM.h + delta
	);
	return delta_rect;
}

Player::CollisionInfo Player::GetCollisionInfo(const Tilemap& tilemap, const Rectangle& rectangle) {
	CollisionInfo info = { false, 0, 0 };
	std::vector<Tilemap::CollisionTile> intersecting_tiles = tilemap.GetCollidingTiles(rectangle);
	for (Tilemap::CollisionTile ctile : intersecting_tiles) {
		if (ctile.tile.GetCollision() == Tile::COLLISION_TYPE::SOLID) {
			info = { true, ctile.row, ctile.col };
			break;
		}
	}
	return info;
}