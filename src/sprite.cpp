#include "sprite.hpp"

#include <stdexcept>
#include "logger.hpp"

Sprite::Sprite() : texture_ (nullptr)
{
}

Sprite::Sprite(Graphics& graphics, SDL_Texture* texture, SDL_FRect default_clip)
	: texture_(texture), default_clip_(default_clip)
{
	if (texture == nullptr) {
		Logger::PrintWarning("Sprite initialized with null texture");
		return;
	}
	SDL_SetTextureScaleMode(texture_, SDL_SCALEMODE_NEAREST);
}

Sprite::Sprite(Graphics& graphics, std::filesystem::path image_path, SDL_FRect default_clip)
	: Sprite(graphics, graphics.LoadTextureFromImage(image_path), default_clip)
{
}

Sprite::Sprite(Graphics& graphics, std::filesystem::path image_path, SDL_FRect default_clip,
	SDL_Point mask_pixel)
	: texture_(graphics.LoadTextureFromImage(image_path, mask_pixel)),
	  default_clip_(default_clip)
{
	SDL_SetTextureScaleMode(texture_, SDL_SCALEMODE_NEAREST);
}

Sprite::Sprite(Graphics& graphics, std::filesystem::path image_path, SDL_FRect default_clip,
	SDL_Color mask_color)
	: texture_(graphics.LoadTextureFromImage(image_path, mask_color)),
	  default_clip_(default_clip)
{
	SDL_SetTextureScaleMode(texture_, SDL_SCALEMODE_NEAREST);
}

SDL_FRect Sprite::GetDefaultClip() const
{
	return default_clip_;
}

void Sprite::SetDefaultClip(const SDL_FRect& clip)
{
	default_clip_ = clip;
}

SDL_FPoint Sprite::GetOrigin() const
{
	return origin_;
}

void Sprite::SetOrigin(const SDL_FPoint& position)
{
	origin_ = position;
}

void Sprite::SetOrigin(Sprite::ORIGIN_ORIENTATION origin_orientation)
{
	// TODO: Make ORIGIN_ORIENTATION bitwise, for optimized calculations
	if (origin_orientation == Sprite::ORIGIN_ORIENTATION::top_left
		|| origin_orientation == Sprite::ORIGIN_ORIENTATION::top_center
		|| origin_orientation == Sprite::ORIGIN_ORIENTATION::top_right)
	{
		origin_.y = 0.0;
		if (origin_orientation == Sprite::ORIGIN_ORIENTATION::top_left)
		{
			origin_.x = 0.0;
		}
		if (origin_orientation == Sprite::ORIGIN_ORIENTATION::top_center)
		{
			origin_.x = default_clip_.w / 2.0;
		}
		if (origin_orientation == Sprite::ORIGIN_ORIENTATION::top_right)
		{
			origin_.x = default_clip_.w;
		}
		return;
	}
	if (origin_orientation == Sprite::ORIGIN_ORIENTATION::center_left
		|| origin_orientation == Sprite::ORIGIN_ORIENTATION::center
		|| origin_orientation == Sprite::ORIGIN_ORIENTATION::center_right)
	{
		origin_.y = default_clip_.h / 2.0;
		if (origin_orientation == Sprite::ORIGIN_ORIENTATION::center_left)
		{
			origin_.x = 0.0;
		}
		if (origin_orientation == Sprite::ORIGIN_ORIENTATION::center)
		{
			origin_.x = default_clip_.w / 2.0;
		}
		if (origin_orientation == Sprite::ORIGIN_ORIENTATION::center_right)
		{
			origin_.x = default_clip_.w;
		}
		return;
	}
	if (origin_orientation == Sprite::ORIGIN_ORIENTATION::bottom_left
		|| origin_orientation == Sprite::ORIGIN_ORIENTATION::bottom_middle
		|| origin_orientation == Sprite::ORIGIN_ORIENTATION::bottom_right)
	{
		origin_.y = default_clip_.h;
		if (origin_orientation == Sprite::ORIGIN_ORIENTATION::bottom_left)
		{
			origin_.x = 0.0;
		}
		if (origin_orientation == Sprite::ORIGIN_ORIENTATION::bottom_middle)
		{
			origin_.x = default_clip_.w / 2.0;
		}
		if (origin_orientation == Sprite::ORIGIN_ORIENTATION::bottom_right)
		{
			origin_.x = default_clip_.w;
		}
		return;
	}
}

SDL_Texture* Sprite::GetTexture() const
{
	return texture_;
}

int Sprite::Draw(Graphics& graphics, SDL_Point position, SDL_FRect clip, const SDL_FlipMode flip) const {
	SDL_FRect destination = {
		round(position.x - origin_.x),
		round(position.y - origin_.y),
		clip.w,
		clip.h};
	if (SDL_RectEmptyFloat(&destination))
	{
		SDL_GetTextureSize(texture_, &(destination.h), &(destination.h));
	}
	if (texture_ == nullptr) {
		SDL_FRect null_clip = { 0, 0, 16, 16 };
		return graphics.DrawTexture(graphics.LoadDefaultTexture(), &null_clip, &destination, flip);
	}
	return graphics.DrawTexture(texture_, &clip, &destination, flip);
}

int Sprite::Draw(Graphics& graphics, SDL_Point position, const SDL_FlipMode flip) const {
	return Draw(graphics, position, default_clip_, flip);
}
