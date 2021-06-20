#pragma once

#include <vector>

struct Options {
	enum class PixelRatio {
		ONE_TO_ONE,
		RETRO
	};
	/// @brief Desired absolute output resolution for different window modes.  e.g., {1920, 1080}, {1280, 720}, {720, 480}
	std::pair<unsigned int, unsigned int> windowed_resolution_desired;
	std::pair<unsigned int, unsigned int> fullscreen_resolution_desired;
	/// @brief Desired gameplay screen ratio.
	PixelRatio pixel_ratio;
	/// @brief Whether the viewport scaler should be floored to an integer or not
	bool forceIntegerScaling;
	double fps_limit;

	void TogglePixelRatio();
	std::pair<float, float> GetViewportRatioFromPixelRatio(PixelRatio pixel_ratio);
};