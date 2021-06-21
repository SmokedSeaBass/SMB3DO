#pragma once

#include <vector>

struct Options {
	enum class PixelRatio {
		ONE_TO_ONE,
		RETRO
	};
	enum class WindowMode {
		WINDOWED,
		FULLSCREEN,
		FULLSCREEN_BORDERLESS
	};
	
	/// @brief Desired absolute output resolution for windowed mode, e.g. {1920, 1080}, {1280, 720}, {720, 480}
	std::pair<unsigned int, unsigned int> windowed_resolution_desired;
	/// @brief Desired absolute output resolution for fullscreen mode, e.g. {1920, 1080}, {1280, 720}, {720, 480}
	std::pair<unsigned int, unsigned int> fullscreen_resolution_desired;

	/// @brief Desired gameplay screen ratio.
	PixelRatio pixel_ratio;
	/// @brief Whether the viewport scaler should be floored to an integer or not
	bool forceIntegerScaling;
	/// @brief Enables/disables "fullscreen" gameplay as oppossed to the conventional 256x224 SNES screen.  Incompatible with RETRO pixel ratio.
	bool enableWidescreen;

	/// @brief Target framerate
	double fps_limit;

	Options();

	void TogglePixelRatio();
	std::pair<float, float> GetViewportRatioFromPixelRatio(PixelRatio pixel_ratio);
};