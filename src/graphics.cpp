#include "graphics.h"

#include <stdexcept>
#include "constants.h"
#include "error.h"
#include "graphics/missingno.xpm"

Graphics::Graphics() {
	renderer_main_ = nullptr;
	window_main_ = nullptr;

	is_fullscreen_ = false;
	current_resolution_ = { 0, 0 };
	viewport_rect_ = { 0, 0, WINDOW_WIDTH_NES, WINDOW_HEIGHT_NES };
	viewport_scaler_ = { 1, 1 };
	viewport_ratio_ = { 8, 7 };
}

Graphics::~Graphics() {
	for (TextureMap::iterator iter = texture_cache_.begin(); iter != texture_cache_.end(); ++iter) {
		SDL_DestroyTexture(iter->second);
	}
	SDL_DestroyRenderer(renderer_main_);
	SDL_DestroyWindow(window_main_);
}

int Graphics::Initialize(Options& options) {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		Error::PrintError("SDL could not initialize: " + std::string(SDL_GetError()));
		return -1;
	}

	if (TTF_Init() < 0) {
		Error::PrintError("SDL_TTF could not initialize: " + std::string(TTF_GetError()));
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
		Error::PrintError("Main window could not be created: " + std::string(SDL_GetError()));
		return -1;
	}

	// Accelerated renderer
	if (SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl") == SDL_FALSE) {
		Error::PrintError("Render driver hint could not be set");
	}
	Uint32 flags = SDL_RENDERER_ACCELERATED;
	if (options.enable_vsync)
		flags |= SDL_RENDERER_PRESENTVSYNC;
	renderer_main_ = SDL_CreateRenderer(window_main_, -1, flags);
	if (renderer_main_ == nullptr) {
		Error::PrintError("Main renderer could not be created: " + std::string(SDL_GetError()));
		return -1;
	}
	
	// Set pixel uspcale, no softening or antialias
	if (SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest") == SDL_FALSE) {
		Error::PrintError("Render scale quality hint could not be set: " + std::string(SDL_GetError()));
		return -1;
	}

	// Allow for colored rect alpha transparency
	SDL_SetRenderDrawBlendMode(renderer_main_, SDL_BLENDMODE_BLEND);

	if (SDL_RenderSetViewport(renderer_main_, &viewport_rect_) < 0) {
		Error::PrintError("Main viewport could not be created: " + std::string(SDL_GetError()));
		return -1;  
	}

	// All drawing should be scaled from a 256x224 canvas
	if (SDL_RenderSetScale(renderer_main_, static_cast<float>(viewport_scaler_.first), static_cast<float>(viewport_scaler_.second)) < 0) {
		Error::PrintError("Main renderer scaling could not be set: " + std::string(SDL_GetError()));
		return -1;
	}

	if (BuildDefaultTexture() < 0) {
		Error::PrintError("Could not build default texture");
		return -1;
	}

	return 0;
}

int Graphics::WindowToggleFullscreen(Options& options) {
	if (!is_fullscreen_) {
		SDL_MaximizeWindow(window_main_);
		SDL_SetWindowSize(window_main_, options.fullscreen_resolution_desired.first, options.fullscreen_resolution_desired.second);
		if (SDL_SetWindowFullscreen(window_main_, SDL_WINDOW_FULLSCREEN) < 0) {
			Error::PrintError("Could not switch to fullscreen mode: " + std::string(SDL_GetError()));
			return -1;
		}
	} else {  
		SDL_SetWindowSize(window_main_, options.windowed_resolution_desired.first, options.windowed_resolution_desired.second);
		if (SDL_SetWindowFullscreen(window_main_, 0) < 0) {
			Error::PrintError("Could not switch to windowed mode: " + std::string(SDL_GetError()));
			return -1;
		}
	}
	is_fullscreen_ = !is_fullscreen_;
	return 0;
}

void Graphics::WindowSetTitle(const std::string& subtitle) {
	auto title = "SMB3DO - v" + std::string(META_VERSION);
	SDL_SetWindowTitle(window_main_, (title + " | " + subtitle).c_str());
}

SDL_DisplayMode Graphics::GetCurrentDisplayMode() {
	SDL_DisplayMode display_mode;
	SDL_GetDesktopDisplayMode(0, &display_mode);		// TODO 7-12-21: Add use thorough displayIndex choosing
	return display_mode;
}

int Graphics::SetViewport(const SDL_Rect* rect) {
	if (rect == NULL) {
		SDL_Rect window_rect = {0, 0, current_resolution_.first, current_resolution_.second };
		return SDL_RenderSetViewport(renderer_main_, &window_rect);
	}
	return SDL_RenderSetViewport(renderer_main_, rect);
}

const SDL_Rect& Graphics::GetViewport() {
	return viewport_rect_;
}

void Graphics::UpdateViewport(Options& options) {
	int window_width, window_height;
	SDL_GL_GetDrawableSize(window_main_, &window_width, &window_height);
	current_resolution_ = { window_width, window_height };
	viewport_ratio_ = options.GetViewportRatioFromPixelRatio(options.pixel_ratio);
	viewport_scaler_ = GetWindowFitViewportScaler(options);
	SDL_RenderSetScale(renderer_main_, viewport_scaler_.first, viewport_scaler_.second);
	if (!options.enable_widescreen) {
		viewport_rect_ = {
			(static_cast<int>(round(current_resolution_.first / viewport_scaler_.first)) - static_cast<int>(WINDOW_WIDTH_NES)) / 2,
			(static_cast<int>(round(current_resolution_.second / viewport_scaler_.second)) - static_cast<int>(WINDOW_HEIGHT_NES)) / 2,
			static_cast<int>(WINDOW_WIDTH_NES),
			static_cast<int>(WINDOW_HEIGHT_NES)
		};
	} else {
		viewport_rect_ = {
			0,
			0,
			static_cast<int>(round(current_resolution_.first / viewport_scaler_.first)),
			static_cast<int>(round(current_resolution_.second / viewport_scaler_.second))
		};
	}
	SetViewport(&viewport_rect_);
	SDL_Rect window_draw_rect = { 0, 0, window_width, window_height };
	SDL_RenderSetClipRect(renderer_main_, &window_draw_rect);
	//Error::PrintDebug("Window Absolute Dimensions: " + std::to_string(current_resolution_.first) + " x " + std::to_string(current_resolution_.second));
	//Error::PrintDebug("Viewport Absolute Dimensions: " + std::to_string(viewport_rect_.w * viewport_scaler_.first) + " x " + std::to_string(viewport_rect_.h * viewport_scaler_.second));
}

std::pair<float, float> Graphics::GetWindowFitViewportScaler(Options& options, SDL_Rect viewport) {
	float x_stretch = ((float)viewport.h * viewport_ratio_.first) / ((float)viewport.w * viewport_ratio_.second);
	float y_scale = (current_resolution_.second / viewport.h);
	if (options.forceIntegerScaling) {
		y_scale = static_cast<float>(floor(y_scale));
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
	texture_cache_[""] = CreateTextureFromSurface(missingno_surface);
	return 0;
}

SDL_Texture* Graphics::GetDefaultTexture() {
	return texture_cache_[""];
}

SDL_Texture* Graphics::CreateTextureFromSurface(SDL_Surface* surface) {
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_main_, surface);
	SDL_FreeSurface(surface);
	return texture;
}

SDL_Texture* Graphics::LoadTextureFromImage(const std::string& file_path) {
	if (texture_cache_.count(file_path) == 0) {
		SDL_Surface* surface = SDL_LoadBMP(file_path.c_str());
		if (surface == NULL) {
			Error::PrintError("Could not load image: \'" + file_path + "\'");
			return nullptr;
		}
		SDL_Texture* texture = CreateTextureFromSurface(surface);
		texture_cache_[file_path] = texture;
	}
	return texture_cache_[file_path];
}

SDL_Texture* Graphics::LoadTextureFromImage(const std::string& file_path, Uint8 red, Uint8 green, Uint8 blue) {
	if (texture_cache_.count(file_path) == 0) {
		SDL_Surface* surface = SDL_LoadBMP(file_path.c_str());
		if (surface == NULL) {
			Error::PrintError("Could not load image: \'" + file_path + "\'");
			return nullptr;
		}
		Uint32 color_key = SDL_MapRGB(surface->format, red, green, blue);
		SDL_SetColorKey(surface, SDL_TRUE, color_key);
		SDL_Texture* texture = CreateTextureFromSurface(surface);
		texture_cache_[file_path] = texture;
	}
	return texture_cache_[file_path];
}

SDL_Texture* Graphics::LoadTextureFromImage(const std::string& file_path, int alpha_x, int alpha_y) {
	if (texture_cache_.count(file_path) == 0) {
		SDL_Surface* surface = SDL_LoadBMP(file_path.c_str());
		if (surface == NULL) {
			Error::PrintError("Could not load image: \'" + file_path + "\'");
			return nullptr;
		}
		Uint32 color_key = 0x00000000;
		if (alpha_x >= 0 && alpha_y >= 0) {
			Uint8 r = 0x00, g = 0x00, b = 0x00;
			Uint32 alpha_pixel = GetSurfacePixel(surface, alpha_x, alpha_y);
			SDL_SetColorKey(surface, SDL_TRUE, alpha_pixel);
		}
		SDL_Texture* texture = CreateTextureFromSurface(surface);
		texture_cache_[file_path] = texture;
	}
	return texture_cache_[file_path];
}

int Graphics::UnloadTexture(SDL_Texture* texture) {
	for (std::pair<std::string, SDL_Texture*> entry : texture_cache_) {
		if (entry.second == texture) {
			SDL_DestroyTexture(entry.second);
			texture_cache_.erase(entry.first);
			return 0;
		}
	}
	Error::PrintWarning("Cannot unload texture " + Error::ptr_to_string(texture) + "; not found in texture cache");
	return -1;
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

int Graphics::DrawColoredRect(const SDL_Rect* rect, Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha) {
	SDL_SetRenderDrawColor(renderer_main_, red, green, blue, alpha);
	return SDL_RenderFillRect(renderer_main_, rect);
}

int Graphics::DrawColoredRect(const Rectangle& rect, Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha) {
	int x = static_cast<int>(floor(rect.x));
	int y = static_cast<int>(floor(rect.y));
	int w = static_cast<int>(ceil(rect.x + rect.w)) - x;
	int h = static_cast<int>(ceil(rect.y + rect.h)) - y;
	SDL_Rect sdl_rect = {
		x, y, w, h
	};
	SDL_SetRenderDrawColor(renderer_main_, red, green, blue, alpha);
	return SDL_RenderFillRect(renderer_main_, &sdl_rect);
}

int Graphics::DrawColoredLine(const std::pair<int, int>& point_1, const std::pair<int, int>& point_2, Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha) {
	SDL_SetRenderDrawColor(renderer_main_, red, green, blue, alpha);
	return SDL_RenderDrawLine(renderer_main_, point_1.first, point_1.second, point_2.first, point_2.second);
}

int Graphics::DrawColoredOutline(const std::pair<int, int>& point_1, const std::pair<int, int>& point_2, Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha) {
	SDL_SetRenderDrawColor(renderer_main_, red, green, blue, alpha);
	SDL_Point points[5] = {
		{point_1.first, point_1.second},
		{point_2.first, point_1.second},
		{point_2.first, point_2.second},
		{point_1.first, point_2.second},
		{point_1.first, point_1.second}
	};
	return SDL_RenderDrawLines(renderer_main_, points, 5);
}
int Graphics::DrawColoredOutline(const Rectangle& rect, Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha) {
	SDL_SetRenderDrawColor(renderer_main_, red, green, blue, alpha);
	SDL_Point points[5] = {
		{static_cast<int>(floor(rect.Left())), static_cast<int>(floor(rect.Top()))},
		{static_cast<int>(ceil(rect.Right())) - 1, static_cast<int>(floor(rect.Top()))},
		{static_cast<int>(ceil(rect.Right())) - 1, static_cast<int>(ceil(rect.Bottom())) - 1},
		{static_cast<int>(floor(rect.Left())), static_cast<int>(ceil(rect.Bottom())) - 1},
		{static_cast<int>(floor(rect.Left())), static_cast<int>(floor(rect.Top()))}
	};
	return SDL_RenderDrawLines(renderer_main_, points, 5);
}

int Graphics::DrawTexture(SDL_Texture* texture, const SDL_Rect* source_rect, const SDL_Rect* dest_rect, const SDL_RendererFlip flip) {
	return SDL_RenderCopyEx(renderer_main_, texture, source_rect, dest_rect, 0.0, NULL, flip);
}

int Graphics::FlipRenderer() {
	SDL_RenderPresent(renderer_main_);
	return 0;
}