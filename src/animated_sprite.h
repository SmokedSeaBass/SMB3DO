#pragma once

#include "sprite.h"

/// @brief Animated Sprite class that extends the standard Sprite class.
///        Animates between source-rect sized subimages from left-to-right.
class AnimatedSprite : public Sprite {
public:
	/// @brief Constructs an AnimatedSprite class.
	/// @param graphics Graphics rendering context to use.
	/// @param file_path File path to the BMP source image.
	/// @param alpha_x X-coordinate of the pixel of the loaded image whose RGB value to treat as transparent. Defaults to -1 (no transparency).
	/// @param alpha_y Y-coordinate of the pixel of the loaded image whose RGB value to treat as transparent. Defaults to -1 (no transparency).
	/// @param source_x Initial x-coordinate of the rect used for rendering the sprite. Defaults to -1 (will not render).
	/// @param source_y Initial y-coordinate of the rect used for rendering the sprite. Defaults to -1 (will not render).
	/// @param source_w Initial width of the rect used for rendering the sprite. Defaults to -1 (will not render).
	/// @param source_h Initial height of the rect used for rendering the sprite. Defaults to -1 (will not render).
	/// @param frame_speed Animation speed, in frames-per-second. Defaults to 1.
	/// @param frame_count Total number of frames in the animation. Defaults to 1.
	/// @param frame_gap Size of the gap, in pixels, between each frame of the animation in the source image. Defaults to 0.
	AnimatedSprite(
		Graphics& graphics,
		const std::string& file_path,
		int alpha_x = -1, int alpha_y = -1,
		int source_x = -1, int source_y = -1,
		int source_w = -1, int source_h = -1,
		double frame_speed = 1, int frame_count = 1, int frame_gap = 0
	);
	~AnimatedSprite();

	void Update(int elapsed_time_ms);

private:
	const int frame_time_;		// Time (in ms) per animation frame
	const int frame_count_;		// Number of total frames in the animation
	const int frame_gap_;		// Pixel gap between each frame of animation
	int current_frame_;			// Current frame in the animation
	int current_frame_time_;	// Time (in ms) elapsed for this current animation frame
};
