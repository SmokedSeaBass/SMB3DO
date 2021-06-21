#include "graphics.h"

#include <stdexcept>
#include "constants.h"
#include "error.h"
#include "graphics/missingno.xpm"

Graphics::Graphics() {
	renderer_main_ = nullptr;
	window_main_ = nullptr;
	default_texture_ = nullptr;

	is_fullscreen_ = false;
	current_resolution_ = { 0, 0 };
	viewport_rect_ = { 0, 0, WINDOW_WIDTH_NES, WINDOW_HEIGHT_NES };
	viewport_scaler_ = { 1, 1 };
	viewport_ratio_ = { 8, 7 };
}

Graphics::~Graphics() {
	SDL_DestroyRenderer(renderer_main_);
	SDL_DestroyWindow(window_main_);
}

int Graphics::Initialize(Options& options) {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::string msg = " SDL could not initialize: " + std::string(SDL_GetError()) + "\n";
		Error::PrintError(std::runtime_error(msg));
		return -1;
	}

	// Initialize graphics variables
	is_fullscreen_ = false;
	current_resolution_ = options.windowed_resolution_desired;
	viewport_ratio_ = options.GetViewportRatioFromPixelRatio(options.pixel_ratio);
	viewport_scaler_ = GetWindowFitViewportScaler(options);
	
	// Main window
	std::string title = "SMB3DO - v" + std::string(META_VERSION);
	window_main_ = SDL_CreateWindow(
		title.c_str(),
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		static_cast<int>(round(current_resolution_.first)), static_cast<int>(round(current_resolution_.second)),
		SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI
	);
	if (window_main_ == nullptr) {
		std::string msg = "Main window could not be created: " + std::string(SDL_GetError());
		Error::PrintError(std::runtime_error(msg));
		return -1;
	}

	// Accelerated renderer
	renderer_main_ = SDL_CreateRenderer(window_main_, -1, SDL_RENDERER_ACCELERATED);
	if (renderer_main_ == nullptr) {
		std::string msg = "Main renderer could not be created: " + std::string(SDL_GetError());
		Error::PrintError(std::runtime_error(msg));
		return -1;
	}
	
	// Pixel uspcale, no softening or antialias
	if (SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest") == SDL_FALSE) {
		std::string msg = "Render scale quality hint could not be set: " + std::string(SDL_GetError());
		Error::PrintError(std::runtime_error(msg));
		return -1;
	}

	// Allow for colored rect alpha transparency
	SDL_SetRenderDrawBlendMode(renderer_main_, SDL_BLENDMODE_BLEND);

	// This should be the last rendering operation done SCALED; the coordinates and rect are real-sized.  Ideally, this would use canvas coordinates.
	if (SDL_RenderSetViewport(renderer_main_, &viewport_rect_) < 0) {
		std::string msg = "Main viewport could not be created: " + std::string(SDL_GetError());
		Error::PrintError(std::runtime_error(msg));
		return -1;  
	}

	// All drawing should be scaled from a 256x224 canvas
	if (SDL_RenderSetScale(renderer_main_, static_cast<float>(viewport_scaler_.first), static_cast<float>(viewport_scaler_.second)) < 0) {
		std::string msg = "Main renderer scaling could not be set: " + std::string(SDL_GetError());
		Error::PrintError(std::runtime_error(msg));
		return -1;
	}

	// Alternative scaling process.  Automatically creates 1-to-1 pixel ratio, instead of 4:3
	//SDL_RenderSetIntegerScale(renderer_main_, SDL_FALSE);
	//SDL_RenderSetLogicalSize(renderer_main_, WINDOW_WIDTH_NES, WINDOW_HEIGHT_NES);

	if (BuildDefaultTexture() < 0) {
		std::string msg = "Could not build default texture";
		Error::PrintError(std::runtime_error(msg));
		return -1;
	}

	return 0;
}

int Graphics::WindowToggleFullscreen(Options& options) {
	if (!is_fullscreen_) {
		SDL_MaximizeWindow(window_main_);
		SDL_SetWindowSize(window_main_, options.fullscreen_resolution_desired.first, options.fullscreen_resolution_desired.second);
		if (SDL_SetWindowFullscreen(window_main_, SDL_WINDOW_FULLSCREEN) < 0) {
			std::string msg = "Could not switch to fullscreen mode: " + std::string(SDL_GetError());
			Error::PrintError(std::runtime_error(msg));
			return -1;
		}
	} else {  
		SDL_SetWindowSize(window_main_, options.windowed_resolution_desired.first, options.windowed_resolution_desired.second);
		if (SDL_SetWindowFullscreen(window_main_, 0) < 0) {
			std::string msg = "Could not switch to windowed mode: " + std::string(SDL_GetError());
			Error::PrintError(std::runtime_error(msg));
			return -1;
		}
	}
	is_fullscreen_ = !is_fullscreen_;
	return 0;
}

void Graphics::WindowSetTitle(std::string& subtitle) {
	auto title = "SMB3DO - v" + std::string(META_VERSION);
	SDL_SetWindowTitle(window_main_, (title + " | " + subtitle).c_str());
}

int Graphics::SetViewport(SDL_Rect& rect) {
	return SDL_RenderSetViewport(renderer_main_, &rect);
}

void Graphics::UpdateViewport(Options& options) {
	viewport_ratio_ = options.GetViewportRatioFromPixelRatio(options.pixel_ratio);

	int window_width, window_height;
	SDL_GL_GetDrawableSize(window_main_, &window_width, &window_height);
	current_resolution_ = { window_width, window_height };

	float viewport_width;
	if (options.enableWidescreen) {
		viewport_width = std::max((double)WINDOW_WIDTH_NES, round(current_resolution_.first / viewport_scaler_.first));
		//viewport_width = floor((float)WINDOW_HEIGHT_NES * (current_resolution_.first / current_resolution_.second));
	} else {
		viewport_width = round((float)WINDOW_WIDTH_NES);
	}

	SDL_Rect viewport_dimensions = { 0, 0, viewport_width, WINDOW_HEIGHT_NES };
	viewport_scaler_ = GetWindowFitViewportScaler(options, viewport_dimensions);
	SDL_RenderSetScale(renderer_main_, viewport_scaler_.first, viewport_scaler_.second);

	viewport_rect_ = {
		std::max((static_cast<int>(round(current_resolution_.first / viewport_scaler_.first)) - static_cast<int>(viewport_width)) / 2, 0),
		std::max((static_cast<int>(round(current_resolution_.second / viewport_scaler_.second)) - static_cast<int>(WINDOW_HEIGHT_NES)) / 2, 0),
		static_cast<int>(viewport_width),
		static_cast<int>(WINDOW_HEIGHT_NES)
	};
	SetViewport(viewport_rect_);

	Error::PrintDebug("Window Absolute Dimensions: " + std::to_string(current_resolution_.first) + " x " + std::to_string(current_resolution_.second));
	Error::PrintDebug("Viewport Absolute Dimensions: " + std::to_string(viewport_rect_.w * viewport_scaler_.first) + " x " + std::to_string(viewport_rect_.h * viewport_scaler_.second));
}

std::pair<float, float> Graphics::GetWindowFitViewportScaler(Options& options, SDL_Rect viewport) {
	float x_stretch = ((float)viewport.h * viewport_ratio_.first) / ((float)viewport.w * viewport_ratio_.second);
	float y_scale = (current_resolution_.second / viewport.h);
	if (options.forceIntegerScaling) {
		y_scale = floor(y_scale);
	}
	float x_scale = y_scale * x_stretch;
	return std::pair<float, float>(x_scale, y_scale);
}

int Graphics::BuildDefaultTexture() {
	SDL_Surface* missingno_surface = IMG_ReadXPMFromArray(missingno_xpm);
	if (missingno_surface == NULL) {
		std::string err = "Could not read XPM: " + std::string(IMG_GetError());
		Error::PrintError(err);
		SDL_FreeSurface(missingno_surface);
		return -1;
	}
	default_texture_ = CreateTextureFromSurface(missingno_surface);
	SDL_FreeSurface(missingno_surface);
	return 0;
}

SDL_Texture* Graphics::GetDefaultTexture() {
	return default_texture_;
}

SDL_Texture* Graphics::CreateTextureFromSurface(SDL_Surface* surface) {
	return SDL_CreateTextureFromSurface(renderer_main_, surface);
}

SDL_Texture* Graphics::CreateTextureFromImage(const std::string& file_path) {
	SDL_Surface* surface = SDL_LoadBMP(file_path.c_str());
	return SDL_CreateTextureFromSurface(renderer_main_, surface);
}

SDL_Texture* Graphics::CreateTextureFromImage(const std::string& file_path, Uint32 color_key) {
	SDL_Surface* surface = SDL_LoadBMP(file_path.c_str());
	SDL_SetColorKey(surface, SDL_TRUE, color_key);
	return SDL_CreateTextureFromSurface(renderer_main_, surface);
}

SDL_Texture* Graphics::CreateTextureFromImage(const std::string& file_path, int alpha_x, int alpha_y) {
	SDL_Surface* surface = SDL_LoadBMP(file_path.c_str());
	Uint32 color_key = 0x00000000;
	if (alpha_x >= 0 && alpha_y >= 0) {
		Uint8 r = 0x00, g = 0x00, b = 0x00;
		Uint32 alpha_pixel = GetSurfacePixel(surface, alpha_x, alpha_y);  // Get RGB values of first pixel
		SDL_GetRGB(alpha_pixel, surface->format, &r, &g, &b);
		color_key = SDL_MapRGB(surface->format, r, g, b);
		SDL_SetColorKey(surface, SDL_TRUE, color_key);
	}
	SDL_SetColorKey(surface, SDL_TRUE, color_key);
	return SDL_CreateTextureFromSurface(renderer_main_, surface);
}

// From StackOverflow: https://stackoverflow.com/questions/53033971/how-to-get-the-color-of-a-specific-pixel-from-sdl-surface
Uint32 Graphics::GetSurfacePixel(SDL_Surface* surface, int x, int y) {
	SDL_LockSurface(surface);
	int bpp = surface->format->BytesPerPixel;
	// Get address of the pixel we want to retrieve
	Uint8* ptr = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;
	Uint32 pixel = 0x00000000;

	switch (bpp) {
	case 1:
		pixel = *ptr;
		break;
	case 2:
		pixel = *(Uint16*)ptr;
		break;
	case 3:
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
			pixel =  ptr[0] << 16 | ptr[1] << 8 | ptr[2];
		} else {
			pixel = ptr[0] | ptr[1] << 8 | ptr[2] << 16;
		}
		break;
	case 4:
		pixel = *(Uint32*)ptr;
		break;
	default:
		break;
	}

	SDL_UnlockSurface(surface);
	return pixel;
}

int Graphics::BlitColoredRect(SDL_Rect* rect, Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha) {
	SDL_SetRenderDrawColor(renderer_main_, red, green, blue, alpha);
	SDL_RenderFillRect(renderer_main_, rect);
	return 0;
}

int Graphics::BlitTexture(SDL_Texture* texture, SDL_Rect* source_rect, SDL_Rect* dest_rect) {
	return SDL_RenderCopy(renderer_main_, texture, source_rect, dest_rect);
}

int Graphics::FlipRenderer() {
	SDL_RenderPresent(renderer_main_);
	return 0;
}