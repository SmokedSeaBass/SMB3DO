#include "player.h"

#include "constants.h"
#include "input.h"

Player::Player(Sprite* sprite, double pos_x, double pos_y) {
	sprite_ = sprite;
	pos_x_ = pos_x;
	pos_y_ = pos_y;
	vel_x_ = 0;
	vel_y_ = 0;
	collider_ = { -5, -14, 10, 13 };  // 0-2 3-12 13-15 on x, 0-3 4-14 15 on y for small mario
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

void Player::Update(Input& input) {
	/* Get input */
	HandleInputs(input);

	/* Apply physics to get intended position */
	// Jumping
	if ((status_ & (char)Status::GROUNDED) && (input.IsButtonPressed(Input::Button::P1_A))) {
		if (abs(vel_x_) > (double)0x0300 / 256.0) vel_y_ = -Physics::JUMP_VEL_SPRINT;
		else if (abs(vel_x_) > (double)0x0200 / 256.0) vel_y_ = -Physics::JUMP_VEL_RUN;
		else if (abs(vel_x_) > (double)0x0100 / 256.0) vel_y_ = -Physics::JUMP_VEL_WALK;
		else vel_y_ = -Physics::JUMP_VEL_STAND;
		status_ &= !(char)Status::GROUNDED;
	}

	// Lateral movement
	if (status_ & (char)Status::GROUNDED) {								// Grounded physics
		if (dpad_vector_[0] == 0) {												// Neutral input, normal deceleration
			if (abs(vel_x_) < Physics::DECEL_GROUND_NORMAL) vel_x_ = 0;
			else vel_x_ -= Physics::DECEL_GROUND_NORMAL * sgn(vel_x_);
		} else if (dpad_vector_[0] == -sgn(vel_x_)) {								// Opposing input, skid deceleration
			vel_x_ += dpad_vector_[0] * Physics::SKID_GROUND_NORMAL;
		} else {																	// Accelerating
			vel_x_ += dpad_vector_[0] * Physics::ACCEL_GROUND;
			double ground_speed_cap = Physics::MAX_SPEED_WALK;
			if (input.IsButtonDown(Input::Button::P1_B)) ground_speed_cap = Physics::MAX_SPEED_RUN;
			if (abs(vel_x_) > ground_speed_cap) {									// Ground x-speed limit
				vel_x_ = ground_speed_cap * sgn(vel_x_);							// TODO: Implement gradual slowing down if cap is exceeded
			}
		}
		// Set air speed cap based on acheived ground speed
		if (abs(vel_x_) <= Physics::MAX_SPEED_WALK) aerial_speed_cap_ = Physics::MAX_SPEED_WALK;
		else if (abs(vel_x_) >= Physics::MAX_SPEED_SPRINT) aerial_speed_cap_ = Physics::MAX_SPEED_SPRINT;
		else aerial_speed_cap_ = Physics::MAX_SPEED_RUN;
	} else {																	// Mid-air physics
		if (dpad_vector_[0] == -sgn(vel_x_)) {										// Mid-air skidding
			vel_x_ += dpad_vector_[0] * Physics::SKID_AIR;
		} else {																	// Mid-air accelerating/neutral
			vel_x_ += dpad_vector_[0] * Physics::ACCEL_AIR;
		}
		if (abs(vel_x_) > aerial_speed_cap_) {
			vel_x_ -= Physics::SKID_AIR * sgn(vel_x_);								// Mid-air x-speed limit (forced deceleration)
		}
	}

	// Gravity
	if (input.IsButtonDown(Input::Button::P1_A) && (vel_y_ < (double)-0x0200 / 256.0)) {
		vel_y_ += Physics::GRAVITY_LIGHT;
	} else {
		vel_y_ += Physics::GRAVITY_HEAVY;
	}
	if (vel_y_ > Physics::VEL_TERM) vel_y_ = Physics::VEL_TERM;

	/* Check collisions and make corrections */
	if (pos_y_ + vel_y_ > 224) {
		pos_y_ = 224;
		vel_y_ = 0;
		status_ |= (char)Status::GROUNDED;
	}


	/* Commit to movement */
	pos_x_ += vel_x_;
	pos_y_ += vel_y_;
}

int Player::Draw(Graphics& graphics) {
	int return_code = sprite_->Draw(graphics, pos_x_, pos_y_);
	SDL_Rect collider_absolute = { collider_.x + static_cast<int>(round(pos_x_)), collider_.y + static_cast<int>(round(pos_y_)), collider_.w, collider_.h };
	//graphics.BlitColoredRect(&collider_absolute, 0xC0, 0x00, 0xC0, 0xCF);
	return return_code;
}