#include "animated_sprite.h"

AnimatedSprite::AnimatedSprite() :
	Sprite(),
	frame_time_(0.0),
	frame_count_(0),
	frame_gap_(0),
	current_frame_(0),
	current_frame_time_(0) {
}

AnimatedSprite::AnimatedSprite(
		Graphics& graphics,
		const std::string& file_path,
		int alpha_x, int alpha_y,
		int source_x, int source_y,
		int source_w, int source_h,
		double frame_speed, int frame_count, int frame_gap) :
	Sprite(graphics, file_path, alpha_x, alpha_y, source_x, source_y, source_w, source_h),
	frame_time_(1000.0 / frame_speed),
	frame_count_(frame_count),
	frame_gap_(frame_gap),
	current_frame_(0),
	current_frame_time_(0) {
}

AnimatedSprite::AnimatedSprite(
		Graphics& graphics,
		SDL_Texture* texture,
		int source_x, int source_y,
		int source_w, int source_h,
		double frame_speed, int frame_count, int frame_gap) :
	Sprite(graphics, texture, source_x, source_y, source_w, source_h),
	frame_time_(1000.0 / frame_speed),
	frame_count_(frame_count),
	frame_gap_(frame_gap),
	current_frame_(0),
	current_frame_time_(0) {
}

AnimatedSprite::~AnimatedSprite() { }

void AnimatedSprite::Update(int elapsed_time_ms) {
	current_frame_time_ += elapsed_time_ms;
	if (current_frame_time_ > frame_time_) {
		current_frame_time_ -= frame_time_;
		if (current_frame_ < frame_count_ - 1) {
			source_rect_.x += source_rect_.w + frame_gap_;
			current_frame_++;
		} else {
			source_rect_.x -= (source_rect_.w + frame_gap_) * (frame_count_ - 1);
			current_frame_ = 0;
		}
	}
}