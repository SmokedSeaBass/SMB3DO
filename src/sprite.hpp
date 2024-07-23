#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include "graphics.hpp"

/// @brief Stores a sprite sheet image and can draw a rectangular section of it.  Stores a sprite
/// sheet (or any image really) as a SDL_Texture. Contains a gettable / settable SDL_Rect that
/// specifies what part of the SDL_Texture to draw when Draw() is called.
class Sprite
{
public:
	enum class ORIGIN_ORIENTATION
	{
		top_left,
		top_center,
		top_right,
		center_left,
		center,
		center_right,
		bottom_left,
		bottom_middle,
		bottom_right
	};

	/// @brief Constructs an empty Sprite object.
	Sprite();

	/// @brief Constructs a Sprite object using a texture source.
	/// @param graphics the graphics rendering context to use.
	/// @param texture a pointer to the SDL_Texture to use as the sprite's source image.
	/// @param default_clip the default region of the sprite to render.
	Sprite(
		Graphics& graphics,
		SDL_Texture* texture,
		SDL_FRect default_clip);
	
	/// @brief Constructs a Sprite object using an image source.
	/// @param graphics the graphics rendering context to use.
	/// @param image_path the filesystem path to the source image.
	/// @param default_clip the default region of the sprite to render.
	Sprite(
		Graphics& graphics,
		std::filesystem::path image_path,
		SDL_FRect default_clip);

	/// @brief Constructs a Sprite object using an image source. Transparency is defined
	/// by a given pixel's color.
	/// @param graphics the graphics rendering context to use.
	/// @param image_path the filesystem path to the source image.
	/// @param default_clip the default region of the sprite texture to render.
	/// @param mask_pixel the pixel whose color should be treated as transparent.
	Sprite(
		Graphics& graphics,
		std::filesystem::path image_path,
		SDL_FRect default_clip,
		SDL_Point mask_pixel);

	/// @brief Constructs a Sprite object using an image source. Transparency is defined
	/// by a given color.
	/// @param graphics the graphics rendering context to use.
	/// @param image_path the filesystem path to the source image.
	/// @param default_clip the default region of the sprite texture to render.
	/// @param mask_pixel the color that should be treated as transparent (alpha is ignored).
	Sprite(
		Graphics& graphics,
		std::filesystem::path image_path,
		SDL_FRect default_clip,
		SDL_Color mask_color);

	SDL_FRect GetDefaultClip() const;
	void SetDefaultClip(const SDL_FRect& clip);


	SDL_FPoint GetOrigin() const;
	void SetOrigin(const SDL_FPoint& position);
	void SetOrigin(ORIGIN_ORIENTATION origin_orientation);

	SDL_Texture* GetTexture() const;

	virtual void ResetAnimation() { }
	virtual void SetAnimationSpeed(double frame_time) { }
	virtual void Update(double delta_time) { }

	int Draw(
		Graphics& graphics,
		SDL_Point position,
		SDL_FRect clip,
		const SDL_FlipMode flip = SDL_FLIP_NONE) const;

	int Draw(
		Graphics& graphics,
		SDL_Point position,
		const SDL_FlipMode flip = SDL_FLIP_NONE) const;

protected:
	SDL_FRect default_clip_;

private:
	SDL_Texture* texture_;
	SDL_FPoint origin_;
};
