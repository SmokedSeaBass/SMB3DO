#include "graphics.h"

#include <stdexcept>
#include "constants.h"
#include "logger.h"
#include "graphics/missingno.xpm"

Graphics::Graphics() {
	renderer_main_ = nullptr;
	window_main_ = nullptr;
	render_canvas_ = nullptr;

	textures_ = TextureCache();
	bitmap_fonts_ = BMPFontCache();
	active_bitmap_font_ = nullptr;

	is_fullscreen_ = false;
	current_resolution_ = { 0, 0 };
	viewport_rect_ = { 0, 0, (int)NES_WINDOW_WIDTH, (int)NES_WINDOW_HEIGHT };
	viewport_scaler_ = { 1, 1 };
	viewport_ratio_ = { 8, 7 };
}

Graphics::~Graphics() {
	for (TextureCache::iterator iter = textures_.begin(); iter != textures_.end(); ++iter) {
		SDL_DestroyTexture(iter->second);
	}
	SDL_DestroyRenderer(renderer_main_);
	SDL_DestroyWindow(window_main_);
}

int Graphics::Initialize(Options& options) {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		Logger::PrintError("SDL could not initialize: " + std::string(SDL_GetError()));
		return -1;
	}

	if (TTF_Init() < 0) {
		Logger::PrintError("SDL_TTF could not initialize: " + std::string(TTF_GetError()));
		return -1;
	}

	// Main window
	std::string title = "SMB3DO - v" + std::string(META_VERSION);
	window_main_ = SDL_CreateWindow(
		title.c_str(),
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		(int)round(options.windowed_resolution_desired.first), (int)round(options.windowed_resolution_desired.second),
		SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI
	);
	if (window_main_ == nullptr) {
		Logger::PrintError("Main window could not be created: " + std::string(SDL_GetError()));
		return -1;
	}

	// Accelerated renderer
	if (SDL_SetHint(SDL_HINT_RENDER_DRIVER, "direct3d") == SDL_FALSE) {
		Logger::PrintError("Render driver hint could not be set");
	}
	Uint32 flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE;
	if (options.enable_vsync)
		flags |= SDL_RENDERER_PRESENTVSYNC;
	renderer_main_ = SDL_CreateRenderer(window_main_, -1, flags);
	if (renderer_main_ == nullptr) {
		Logger::PrintError("Main renderer could not be created: " + std::string(SDL_GetError()));
		return -1;
	}

	// Set pixel uspcale, no softening or antialias
	if (SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest") == SDL_FALSE) {
		Logger::PrintError("Render scale quality hint could not be set: " + std::string(SDL_GetError()));
		return -1;
	}

	// Allow for colored rect alpha transparency
	SDL_SetRenderDrawBlendMode(renderer_main_, SDL_BLENDMODE_BLEND);

	// Create render canvas texture
	render_canvas_ = SDL_CreateTexture(renderer_main_, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, NES_WINDOW_WIDTH, NES_WINDOW_HEIGHT);
	if (render_canvas_ == NULL) {
		Logger::PrintError("Could not create canvas texture: " + std::string(SDL_GetError()));
		return -1;
	}
	SDL_SetRenderTarget(renderer_main_, render_canvas_);

	UpdateViewport(options);
	UpdateCanvas(options);

	if (BuildDefaultTexture() < 0) {
		Logger::PrintError("Could not build default texture");
		return -1;
	}

	return 0;
}

int Graphics::WindowToggleFullscreen(Options& options) {
	if (!is_fullscreen_) {
		SDL_MaximizeWindow(window_main_);
		SDL_SetWindowSize(window_main_, options.fullscreen_resolution_desired.first, options.fullscreen_resolution_desired.second);
		if (SDL_SetWindowFullscreen(window_main_, SDL_WINDOW_FULLSCREEN) < 0) {
			Logger::PrintError("Could not switch to fullscreen mode: " + std::string(SDL_GetError()));
			return -1;
		}
	} else {  
		SDL_SetWindowSize(window_main_, options.windowed_resolution_desired.first, options.windowed_resolution_desired.second);
		if (SDL_SetWindowFullscreen(window_main_, 0) < 0) {
			Logger::PrintError("Could not switch to windowed mode: " + std::string(SDL_GetError()));
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
	if (!options.enable_widescreen) {
		viewport_rect_ = {
			(int)round((current_resolution_.first - ((int)NES_WINDOW_WIDTH * viewport_scaler_.first)) / 2),
			(int)round((current_resolution_.second - ((int)NES_WINDOW_HEIGHT * viewport_scaler_.second)) / 2),
			(int)round((int)NES_WINDOW_WIDTH * viewport_scaler_.first),
			(int)round((int)NES_WINDOW_HEIGHT * viewport_scaler_.second)
		};
	} else {
		viewport_rect_ = {
			0,
			0,
			(int)round(current_resolution_.first),
			(int)round(current_resolution_.second)
		};
	}
	//Logger::PrintDebug("Window Absolute Dimensions: " + std::to_string(current_resolution_.first) + " x " + std::to_string(current_resolution_.second));
	//Logger::PrintDebug("Viewport Absolute Dimensions: " + std::to_string(viewport_rect_.w * viewport_scaler_.first) + " x " + std::to_string(viewport_rect_.h * viewport_scaler_.second));
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

void Graphics::UpdateCanvas(Options& options) {
	SDL_DestroyTexture(render_canvas_);
	if (options.enable_widescreen) {
		float extended_width = NES_WINDOW_HEIGHT * (current_resolution_.first / current_resolution_.second) * (8.0 / 7.0) * ((float)viewport_ratio_.second / (float)viewport_ratio_.first);
		render_canvas_ = SDL_CreateTexture(renderer_main_, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, extended_width, NES_WINDOW_HEIGHT);
	} else {
		render_canvas_ = SDL_CreateTexture(renderer_main_, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, NES_WINDOW_WIDTH, NES_WINDOW_HEIGHT);
	}
	SDL_SetRenderTarget(renderer_main_, render_canvas_);
}

SDL_Rect Graphics::GetCanvasDimensions() {
	int width, height;
	SDL_QueryTexture(render_canvas_, NULL, NULL, &width, &height);
	return { 0, 0, width, height };
}

int Graphics::BuildDefaultTexture() {
	SDL_Surface* missingno_surface = IMG_ReadXPMFromArray(missingno_xpm);
	if (missingno_surface == NULL) {
		std::string err = "Graphics building default texture: Could not read XPM: " + std::string(IMG_GetError());
		Logger::PrintError(err);
		SDL_FreeSurface(missingno_surface);
		return -1;
	}
	textures_[""] = CreateTextureFromSurface(missingno_surface);
	return 0;
}

SDL_Texture* Graphics::GetDefaultTexture() {
	return textures_[""];
}

SDL_Texture* Graphics::CreateTextureFromSurface(SDL_Surface* surface) {
	if (surface == nullptr) {
		Logger::PrintError("Graphics creating texture from surface: surface pointer is NULL");
		return nullptr;
	}
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_main_, surface);
	SDL_FreeSurface(surface);
	return texture;
}

SDL_Texture* Graphics::LoadTextureFromImage(const std::string& file_path) {
	if (textures_.count(file_path) == 0) {
		SDL_Surface* surface = SDL_LoadBMP(file_path.c_str());
		if (surface == NULL) {
			Logger::PrintError("Graphics loading texture from image \'" + file_path + "\': Could not load image");
			return nullptr;
		}
		SDL_Texture* texture = CreateTextureFromSurface(surface);
		textures_[file_path] = texture;
	}
	return textures_[file_path];
}

SDL_Texture* Graphics::LoadTextureFromImage(const std::string& file_path, Uint8 red, Uint8 green, Uint8 blue) {
	if (textures_.count(file_path) == 0) {
		SDL_Surface* surface = SDL_LoadBMP(file_path.c_str());
		if (surface == NULL) {
			Logger::PrintError("Graphics loading texture from image \'" + file_path + "\': Could not load image");
			return nullptr;
		}
		Uint32 color_key = SDL_MapRGB(surface->format, red, green, blue);
		SDL_SetColorKey(surface, SDL_TRUE, color_key);
		SDL_Texture* texture = CreateTextureFromSurface(surface);
		textures_[file_path] = texture;
	}
	return textures_[file_path];
}

SDL_Texture* Graphics::LoadTextureFromImage(const std::string& file_path, int alpha_x, int alpha_y) {
	if (textures_.count(file_path) == 0) {
		SDL_Surface* surface = SDL_LoadBMP(file_path.c_str());
		if (surface == NULL) {
			Logger::PrintError("Graphics loading texture from image \'" + file_path + "\': Could not load image");
			return nullptr;
		}
		Uint32 color_key = 0x00000000;
		if (alpha_x >= 0 && alpha_y >= 0) {
			Uint8 r = 0x00, g = 0x00, b = 0x00;
			Uint32 alpha_pixel = GetSurfacePixel(surface, alpha_x, alpha_y);
			SDL_SetColorKey(surface, SDL_TRUE, alpha_pixel);
		}
		SDL_Texture* texture = CreateTextureFromSurface(surface);
		textures_[file_path] = texture;
	}
	return textures_[file_path];
}

int Graphics::UnloadTexture(SDL_Texture* texture) {
	for (std::pair<std::string, SDL_Texture*> entry : textures_) {
		if (entry.second == texture) {
			SDL_DestroyTexture(entry.second);
			textures_.erase(entry.first);
			return 0;
		}
	}
	Logger::PrintWarning("Graphics unloading texture " + Logger::ptr_to_string(texture) + ": texture not found in texture cache");
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
	int x = (int)floor(rect.x);
	int y = (int)floor(rect.y);
	int w = (int)ceil(rect.x + rect.w) - x;
	int h = (int)ceil(rect.y + rect.h) - y;
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

int Graphics::LoadBMPFont(const std::string& path_to_bmp, unsigned int glyph_width, unsigned int glyph_height, const std::string& font_name) {
	std::string font_index = font_name;
	if (font_index == "") {
		font_index = path_to_bmp;
	}
	if (bitmap_fonts_.count(font_index) == 0) {
		BitmapFont font = BitmapFont(*this, path_to_bmp, glyph_width, glyph_height);
		bitmap_fonts_[font_index] = std::make_unique<BitmapFont>(font);
		return 0;
	}
	Logger::PrintWarning("Loading BitmapFont \'" + font_name + "\': BitmapFont already loaded");
	return 0;
}

int Graphics::SetTextFont(const std::string& font_name) {
	if (bitmap_fonts_.count(font_name) == 0) {
		Logger::PrintError("Setting text to BitmapFont \'" + font_name + "\': BitmapFont not loaded");
		return -1;
	}
	active_bitmap_font_ = bitmap_fonts_[font_name].get();
	return 0;
}

int Graphics::DrawText(const std::string& text, int pos_x, int pos_y) {
	if (active_bitmap_font_ == nullptr) {
		Logger::PrintError("Drawing text: Active BitmapFont is NULL or unset");
		return -1;
	}
	return active_bitmap_font_->DrawText(*this, text, pos_x, pos_y);
	return 0;
}

void Graphics::PresentRender() {
	// Set render target to main window
	SDL_SetRenderTarget(renderer_main_, NULL);
	DrawColoredRect(NULL, 0x00, 0x00, 0x00, 0xFF);
	// Copy canvas to new render target
	SDL_RenderCopy(renderer_main_, render_canvas_, NULL, &viewport_rect_);
	// Present render
	SDL_RenderPresent(renderer_main_);
	// Set render target back to canvas
	SDL_SetRenderTarget(renderer_main_, render_canvas_);
}