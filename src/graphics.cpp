#include "graphics.h"

#include <stdexcept>
#include "constants.h"
#include "error.h"

Graphics::Graphics() {
	is_fullscreen_ = false;
	renderer_main_ = nullptr;
	window_main_ = nullptr;
}

Graphics::~Graphics() {
	SDL_DestroyRenderer(renderer_main_);
	SDL_DestroyWindow(window_main_);
}

int Graphics::Initialize() {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::string msg = " SDL could not initialize: " + std::string(SDL_GetError()) + "\n";
		Error::PrintError(std::runtime_error(msg));
		return -1;
	}

	window_main_ = SDL_CreateWindow(
		"SMB3DO - v" META_VERSION,
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		WINDOW_WIDTH, WINDOW_HEIGHT,
		SDL_WINDOW_SHOWN
	);
	if (window_main_ == nullptr) {
		std::string msg = "Main window could not be created: " + std::string(SDL_GetError());
		Error::PrintError(std::runtime_error(msg));
		return -1;
	}

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

	//SDL_SetRenderDrawBlendMode(renderer_main_, SDL_BLENDMODE_BLEND);  // Allow for colored rect alpha transparency

	// This should be the last rendering operation done SCALED; the coordinates and rect are real-sized.  Ideally, this would use canvas coordinates.
	if (SDL_RenderSetViewport(renderer_main_, &ViewportRects::screen_main_rect) < 0) {
		std::string msg = "Main viewport could not be created: " + std::string(SDL_GetError());
		Error::PrintError(std::runtime_error(msg));
		return -1;  
	} 

	Error::PrintDebug("Window Absolute Dimensions: " + std::to_string(WINDOW_WIDTH) + " x " + std::to_string(WINDOW_HEIGHT));
	Error::PrintDebug("Viewport Absolute Dimensions: " + std::to_string(ViewportRects::screen_main_rect.w * SCREEN_SCALE_X) + " x " + std::to_string(ViewportRects::screen_main_rect.h * SCREEN_SCALE_Y));

	// All drawing should be scaled from a 256x224 canvas
	if (SDL_RenderSetScale(renderer_main_, static_cast<float>(SCREEN_SCALE_X), static_cast<float>(SCREEN_SCALE_Y)) < 0) {
		std::string msg = "Main renderer scaling could not be set: " + std::string(SDL_GetError());
		Error::PrintError(std::runtime_error(msg));
		return -1;
	}

	// Alternative scaling process.  Automatically creates 1-to-1 pixel ratio, instead of 4:3
	//SDL_RenderSetIntegerScale(renderer_main_, SDL_FALSE);
	//SDL_RenderSetLogicalSize(renderer_main_, WINDOW_WIDTH_NES, WINDOW_HEIGHT_NES);

	return 0;
}

int Graphics::WindowToggleFullscreen() {
	is_fullscreen_ = !is_fullscreen_;
	if (is_fullscreen_) {
		if (SDL_SetWindowFullscreen(window_main_, SDL_WINDOW_FULLSCREEN) < 0) {
			std::string msg = "Could not switch to fullscreen mode: " + std::string(SDL_GetError());
			Error::PrintError(std::runtime_error(msg));
			return -1;
		}
	} else {
		if (SDL_SetWindowFullscreen(window_main_, 0) < 0) {
			std::string msg = "Could not switch to windowed mode: " + std::string(SDL_GetError());
			Error::PrintError(std::runtime_error(msg));
			return -1;
		}
	}
	return 0;
}

void Graphics::WindowSetTitle(std::string& subtitle) {
	std::string title = "SMB3DO - v" META_VERSION;
	SDL_SetWindowTitle(window_main_, (title + " | " + subtitle).c_str());
}

int Graphics::SetViewport(SDL_Rect& rect) {
	return SDL_RenderSetViewport(renderer_main_, &rect);
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

SDL_Rect Graphics::ViewportRects::screen_main_rect = {
	(static_cast<int>(round(WINDOW_WIDTH / SCREEN_SCALE_X)) - WINDOW_WIDTH_NES) / 2,
	(static_cast<int>(round(WINDOW_HEIGHT / SCREEN_SCALE_Y)) - WINDOW_HEIGHT_NES) / 2,
	WINDOW_WIDTH_NES,
	WINDOW_HEIGHT_NES
};