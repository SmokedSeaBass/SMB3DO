#pragma once

#include "sprite.hpp"


/// @brief Animated Sprite class that extends the standard Sprite class.
///        Animates between source-rect sized subimages from left-to-right.
class AnimatedSprite : public Sprite
{
public:
	/// @brief Constructs an empty AnimatedSprite object.
	AnimatedSprite() = default;

	/// @brief Constructs an AnimatedSprite object from an image source.
	/// @param graphics the graphics rendering context to use.
	/// @param image_path the filesystem path to the source image.
	/// @param mask_pixel the pixel whose color should be treated as transparent in
	/// the source image.
	/// @param initial_clip the first animation frame of the sprite's image to render.
	/// @param frame_speed the animation speed, in frames per second.
	/// @param frame_count the total number of frames in the animation.
	/// @param frame_gap the size of the gap, in pixels, between each frame of the animation in
	/// the source image.
	AnimatedSprite(
		Graphics& graphics,
		const std::filesystem::path& image_path,
		SDL_Point mask_pixel,
		SDL_FRect clip,
		double frame_speed = 15,
		int frame_count = 1,
		int frame_gap = 0);
	/// @brief Constructs an AnimatedSprite object from a texture source.
	/// @param graphics the graphics rendering context to use.
	/// @param texture a pointer to the SDL_Texture to use as the sprite's source image.
	/// @param initial_clip the first animation frame of the sprite's texture to render.
	/// @param frame_speed the animation speed, in frames per second.
	/// @param frame_count the total number of frames in the animation.
	/// @param frame_gap the size of the gap, in pixels, between each frame of the animation in
	/// the source texture.
	AnimatedSprite(
		Graphics& graphics,
		SDL_Texture* texture,
		SDL_FRect initial_clip,
		double frame_speed = 15,
		int frame_count = 1,
		int frame_gap = 0);
	~AnimatedSprite();

	void ResetAnimation() override;
	void SetAnimationSpeed(double frame_time) override;
	void Update(double delta_time) override;

private:
	double frame_time_;			// Time (in milliseconds) per animation frame
	const int frame_count_;		// Number of total frames in the animation
	const int frame_gap_;		// Pixel gap between each frame of animation
	int current_frame_;			// Current frame number in the animation
	double current_frame_time_;	// Time (in milliseconds) elapsed for this current animation frame
};
