#include "animated_sprite.hpp"

AnimatedSprite::AnimatedSprite() : Sprite()
{
}


AnimatedSprite::AnimatedSprite(
	Graphics& graphics,
	const std::filesystem::path& image_path,
	SDL_Point mask_pixel,
	SDL_FRect initial_clip,
	double frame_speed, int frame_count, int frame_gap)
	: Sprite(graphics, image_path, initial_clip, mask_pixel),
	  frame_time_(1000.0 / frame_speed),
	  frame_count_(frame_count),
	  frame_gap_(frame_gap),
	  current_frame_(0),
	  current_frame_time_(0)
{
}

AnimatedSprite::AnimatedSprite(
	Graphics& graphics,
	SDL_Texture* texture,
	SDL_FRect initial_clip,
	double frame_speed, int frame_count, int frame_gap)
	: Sprite(graphics, texture, initial_clip),
	  frame_time_(1000.0 / frame_speed),
	  frame_count_(frame_count),
	  frame_gap_(frame_gap),
	  current_frame_(0),
	  current_frame_time_(0)
{
}

AnimatedSprite::~AnimatedSprite()
{
}

void AnimatedSprite::ResetAnimation()
{
	default_clip_.x -= (default_clip_.w + frame_gap_) * current_frame_;
	current_frame_ = 0;
	current_frame_time_ = 0;
}

void AnimatedSprite::SetAnimationSpeed(double frame_time)
{
	frame_time_ = frame_time;
}

void AnimatedSprite::Update(double delta_time)
{
	current_frame_time_ += delta_time;
	if (current_frame_time_ > frame_time_)
	{
		current_frame_time_ -= frame_time_;
		if (current_frame_ < frame_count_ - 1)
		{
			default_clip_.x += default_clip_.w + frame_gap_;
			current_frame_++;
		}
		else
		{
			default_clip_.x -= (default_clip_.w + frame_gap_) * (frame_count_ - 1);
			current_frame_ = 0;
		}
	}
}