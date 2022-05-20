#include "options.h"


Options::Options() {
	fps_limit = 0;
	enable_vsync = false;
	interp_factor = 1;
	fullscreen_resolution_desired = { 2560, 1440 };
	windowed_resolution_desired = { 640, 480 };
	pixel_ratio = Options::PixelRatio::RETRO;
	forceIntegerScaling = false;
	enable_widescreen = false;
}


void Options::TogglePixelRatio() {
	switch (pixel_ratio) {
	case Options::PixelRatio::ONE_TO_ONE:
		pixel_ratio = Options::PixelRatio::RETRO;
		break;
	case Options::PixelRatio::RETRO:
		pixel_ratio = Options::PixelRatio::ONE_TO_ONE;
		break;
	}
	return;
}

std::pair<float, float> Options::GetViewportRatioFromPixelRatio(PixelRatio pixel_ratio) {
	std::pair<float, float> viewport_ratio;
	switch (pixel_ratio) {
	case Options::PixelRatio::ONE_TO_ONE:
		viewport_ratio = { 8, 7 };
		break;
	case Options::PixelRatio::RETRO:
		viewport_ratio = { 4, 3 };
		break;
	}
	return viewport_ratio;
}