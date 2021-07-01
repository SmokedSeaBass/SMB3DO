#include "player.h"

#include <assert.h>
#include "constants.h"
#include "error.h"
#include "game.h"

const Rectangle COLLIDER_X(-5, -10, 10, 7);
const Rectangle COLLIDER_Y(-4, -12, 8, 11);
const Rectangle COLLIDER_Y_UP(-1, -12, 2, 11);		// Special collider that's skinnier and causes easier sideways "sliding" 

Player::Player(Sprite* sprite, double pos_x, double pos_y) {
	sprite_ = sprite;
	pos_x_ = pos_x;
	pos_y_ = pos_y;
	vel_x_ = 0;
	vel_y_ = 0;
	collider_ = { -5, -13, 10, 9 };  // 0-2 [3-12] 13-15 on x, 0-1 [2-14] 15 on y for small mario
	speed_grounded_ = 0;
	aerial_speed_cap_ = Physics::MAX_SPEED_RUN;
	status_ = 0;
	dpad_vector_ = { 0, 0 };
}

Player::~Player() { }

void Player::HandleInputs(Input& input) {
	dpad_vector_ = { 0, 0 };
	if (input.IsButtonDown(Input::Button::P1_RIGHT)) dpad_vector_[0] += 1;
	if (input.IsButtonDown(Input::Button::P1_UP)) dpad_vector_[1] += -1;
	if (input.IsButtonDown(Input::Button::P1_LEFT)) dpad_vector_[0] += -1;
	if (input.IsButtonDown(Input::Button::P1_DOWN)) dpad_vector_[1] += 1;
}

void Player::Update(Input& input, int delta_time, const Tilemap& tilemap) {
	/* Get input */
	HandleInputs(input);

	/* Apply physics to get intended position */
	// TODO 6-28-21: Proper variable delta_time physics calculations

	// Jumping
	if ((status_ & (unsigned char)Status::GROUNDED) && (input.IsButtonPressed(Input::Button::P1_A))) {
		if (abs(vel_x_) > (double)0x0300 * Game::fps_ratio / 256.0) vel_y_ = -Physics::JUMP_VEL_SPRINT * Game::fps_ratio;
		else if (abs(vel_x_) > (double)0x0200 * Game::fps_ratio / 256.0) vel_y_ = -Physics::JUMP_VEL_RUN * Game::fps_ratio;
		else if (abs(vel_x_) > (double)0x0100 * Game::fps_ratio / 256.0) vel_y_ = -Physics::JUMP_VEL_WALK * Game::fps_ratio;
		else vel_y_ = -Physics::JUMP_VEL_STAND * Game::fps_ratio;
		status_ &= ~(unsigned char)Status::GROUNDED;
	}

	// Lateral movement
	if (status_ & (unsigned char)Status::GROUNDED) {								// Grounded physics
		if (dpad_vector_[0] == 0) {												// Neutral input, normal deceleration
			if (abs(vel_x_) < Physics::DECEL_GROUND_NORMAL * Game::fps_ratio * Game::fps_ratio) vel_x_ = 0;
			else vel_x_ -= Physics::DECEL_GROUND_NORMAL * Game::fps_ratio * Game::fps_ratio * sgn(vel_x_);
		} else if (dpad_vector_[0] == -sgn(vel_x_)) {								// Opposing input, skid deceleration
			vel_x_ += dpad_vector_[0] * Physics::SKID_GROUND_NORMAL * Game::fps_ratio * Game::fps_ratio;
		} else {																	// Accelerating
			vel_x_ += dpad_vector_[0] * Physics::ACCEL_GROUND * Game::fps_ratio * Game::fps_ratio;
			double ground_speed_cap = Physics::MAX_SPEED_WALK * Game::fps_ratio;
			if (input.IsButtonDown(Input::Button::P1_B)) ground_speed_cap = Physics::MAX_SPEED_RUN * Game::fps_ratio;
			if (abs(vel_x_) > ground_speed_cap) {									// Ground x-speed limit
				vel_x_ = ground_speed_cap * sgn(vel_x_);							// TODO: Implement gradual slowing down if cap is exceeded
			}
		}
		// Set air speed cap based on acheived ground speed
		//if (abs(vel_x_) <= Physics::MAX_SPEED_WALK * Game::fps_ratio) aerial_speed_cap_ = Physics::MAX_SPEED_WALK * Game::fps_ratio;
		//else if (abs(vel_x_) >= Physics::MAX_SPEED_SPRINT * Game::fps_ratio) aerial_speed_cap_ = Physics::MAX_SPEED_SPRINT * Game::fps_ratio;
		//else aerial_speed_cap_ = Physics::MAX_SPEED_RUN * Game::fps_ratio;
		aerial_speed_cap_ = Physics::MAX_SPEED_RUN * Game::fps_ratio;
	} else {																	// Mid-air physics
		if (dpad_vector_[0] == -sgn(vel_x_)) {										// Mid-air skidding
			vel_x_ += dpad_vector_[0] * Physics::SKID_AIR * Game::fps_ratio * Game::fps_ratio;
		} else {																	// Mid-air accelerating/neutral
			vel_x_ += dpad_vector_[0] * Physics::ACCEL_AIR * Game::fps_ratio * Game::fps_ratio;
		}
		if (abs(vel_x_) > aerial_speed_cap_) {
			vel_x_ -= Physics::SKID_AIR * Game::fps_ratio * sgn(vel_x_);								// Mid-air x-speed limit (forced deceleration)
		}
	}

	// Gravity
	if (input.IsButtonDown(Input::Button::P1_A) && (vel_y_ < (double)-0x0200 * Game::fps_ratio / 256.0)) {
		vel_y_ += Physics::GRAVITY_LIGHT * Game::fps_ratio * Game::fps_ratio;
	} else {
		vel_y_ += Physics::GRAVITY_HEAVY * Game::fps_ratio * Game::fps_ratio;
	}
	if (vel_y_ > Physics::VEL_TERM * Game::fps_ratio) vel_y_ = Physics::VEL_TERM * Game::fps_ratio;

	/* Check collisions and make corrections */
	//Error::PrintDebug("X:  " + std::to_string(pos_x_) + ", Y:  " + std::to_string(pos_y_));
	//Error::PrintDebug("VX: " + std::to_string(vel_x_) + ", VY: " + std::to_string(vel_y_));
	//Error::PrintDebug("XL:  " + std::to_string(pos_x_ + COLLIDER_X.Left()) + ", XR:  " + std::to_string(pos_x_ + COLLIDER_X.Right()));
	//Error::PrintDebug("YL:  " + std::to_string(TopCollision(0).Left()) + ", YR:  " + std::to_string(TopCollision(0).Right()));
	double delta_x = vel_x_;
	double delta_y = vel_y_;
	if (delta_x >= 0) {
		// React to collision RIGHT
		CollisionInfo info = GetCollisionInfo(tilemap, RightCollision(delta_x));
		if (info.collided) {
			pos_x_ = info.col * TILESIZE_NES - COLLIDER_X.Right();
			vel_x_ = 0;
		} else {
			pos_x_ += delta_x;
		}
		// Check for collision in opposite direction
		info = GetCollisionInfo(tilemap, LeftCollision(0));
		if (info.collided) {
			pos_x_ = (info.col + 1) * TILESIZE_NES - COLLIDER_X.Left();
		}
	}
	if (delta_x <= 0) {
		// React to collision LEFT
		CollisionInfo info = GetCollisionInfo(tilemap, LeftCollision(delta_x));
		if (info.collided) {
			pos_x_ = (info.col + 1) * TILESIZE_NES - COLLIDER_X.Left();
			vel_x_ = 0;
		} else {
			pos_x_ += delta_x;
		}
		// Check for collision in opposite direction
		info = GetCollisionInfo(tilemap, RightCollision(0));
		if (info.collided) {
			pos_x_ = info.col * TILESIZE_NES - COLLIDER_X.Right();
		}
	}
	if (delta_y > 0) {
		// React to collision DOWN
		CollisionInfo info = GetCollisionInfo(tilemap, BottomCollision(delta_y));
		if (info.collided) {
			pos_y_ = info.row * TILESIZE_NES - COLLIDER_Y.Bottom();
			vel_y_ = 0;
			status_ |= (unsigned char)Status::GROUNDED;
		} else {
			pos_y_ += delta_y;
			status_ &= ~(unsigned char)Status::GROUNDED;
		}
		// Check for collision in opposite direction
		info = GetCollisionInfo(tilemap, TopCollision(0));
		if (info.collided) {
			pos_y_ = (info.row + 1) * TILESIZE_NES - COLLIDER_Y_UP.Top();
		}
	} else {
		// React to collision UP
		CollisionInfo info = GetCollisionInfo(tilemap, TopCollision(delta_y));
		if (info.collided) {
			pos_y_ = (info.row + 1) * TILESIZE_NES - COLLIDER_Y_UP.Top();
			vel_y_ = 0;
		} else {
			pos_y_ += delta_y;
		}
		// Check for collision in opposite direction
		info = GetCollisionInfo(tilemap, BottomCollision(0));
		if (info.collided) {
			pos_y_ = info.row * TILESIZE_NES - COLLIDER_Y.Bottom();
			status_ |= (unsigned char)Status::GROUNDED;
		}
	}
}

int Player::Draw(Graphics& graphics) {
	double pos_x = floor(pos_x_), pos_y = floor(pos_y_);
	int return_code = sprite_->Draw(graphics, static_cast<int>(pos_x), static_cast<int>(pos_y));
	if (DEBUG_SHOW_HITBOXES) {
		//graphics.DrawColoredOutline({ COLLIDER_X.Left() + pos_x_, COLLIDER_X.Top() + pos_y_ }, { COLLIDER_X.Right() + pos_x_, COLLIDER_X.Bottom() + pos_y_ }, 0xFF, 0x6B, 0xFA, 0x9F);
		//graphics.DrawColoredOutline({ COLLIDER_Y.Left() + pos_x_, COLLIDER_Y.Top() + pos_y_ }, { COLLIDER_Y.Right() + pos_x_ - 1, COLLIDER_Y.Bottom() + pos_y_ - 1}, 0xA6, 0xFF, 0x70, 0x9F);
		graphics.DrawColoredRect(RightCollision(0), 0xFF, 0x15, 0x00, 0xAF);
		graphics.DrawColoredRect(LeftCollision(0), 0xA6, 0xFF, 0x70, 0xAF);
		graphics.DrawColoredRect(BottomCollision(0), 0x00, 0x4C, 0xFF, 0xAF);
		graphics.DrawColoredRect(TopCollision(0), 0xFF, 0x6B, 0xFA, 0xAF);
		//graphics.DrawColoredRect(Rectangle( pos_x_ + COLLIDER_Y.x, pos_y_ + COLLIDER_Y.y, COLLIDER_Y.w, COLLIDER_Y.h ), 0x00, 0x4C, 0xFF, 0xAF);
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
		floor(pos_x_) + COLLIDER_X.Left() + delta,
		floor(pos_y_) + COLLIDER_X.Top(),
		COLLIDER_X.w / 2.0 - delta,
		COLLIDER_X.h
	);
	return delta_rect;
}

Rectangle Player::RightCollision(double delta) const {
	assert(delta >= 0);
	Rectangle delta_rect(
		floor(pos_x_) + COLLIDER_X.Left() + COLLIDER_X.w / 2.0,
		floor(pos_y_) + COLLIDER_X.Top(),
		COLLIDER_X.w / 2.0 + delta,
		COLLIDER_X.h
	);
	return delta_rect;
}

Rectangle Player::TopCollision(double delta) const {
	assert(delta <= 0);
	Rectangle delta_rect(
		floor(pos_x_) + COLLIDER_Y_UP.Left(),
		floor(pos_y_) + COLLIDER_Y_UP.Top() + delta,
		COLLIDER_Y_UP.w,
		COLLIDER_Y_UP.h / 2.0 - delta
	);
	return delta_rect;
}

Rectangle Player::BottomCollision(double delta) const {
	assert(delta >= 0);
	Rectangle delta_rect(
		floor(pos_x_) + COLLIDER_Y.Left(),
		floor(pos_y_) + COLLIDER_Y.Top() + COLLIDER_Y.h / 2.0,
		COLLIDER_Y.w,
		COLLIDER_Y.h / 2.0 + delta
	);
	return delta_rect;
}

Player::CollisionInfo Player::GetCollisionInfo(const Tilemap& tilemap, const Rectangle& rectangle) {
	CollisionInfo info = { false, 0, 0 };
	std::vector<Tilemap::CollisionTile> tiles = tilemap.GetCollidingTiles(rectangle);
	for (Tilemap::CollisionTile ctile : tiles) {
		if (ctile.tile.GetCollision() == Tile::COLLISION_TYPE::SOLID) {
			info = { true, ctile.row, ctile.col };
			break;
		}
	}
	return info;
}