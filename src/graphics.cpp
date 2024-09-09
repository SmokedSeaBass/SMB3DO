#include "graphics.hpp"

#include <stdexcept>
#include "constants.hpp"
#include "logger.hpp"

Graphics::Graphics()
  : renderer_(nullptr),
	window_(nullptr),
	render_canvas_(nullptr),
	textures_(TextureCache()),
	bitmap_fonts_(BitmapFontCache()),
	active_bitmap_font_(nullptr),
	is_fullscreen_(false),
	current_resolution_({0, 0}),
	viewport_({0.0, 0.0, NES_WINDOW_WIDTH, NES_WINDOW_HEIGHT}),
	viewport_scaler_({1.0, 1.0}),
	viewport_ratio_({8, 7})
{
}

Graphics::~Graphics()
{
	for (TextureCache::iterator iter = textures_.begin(); iter != textures_.end(); ++iter)
	{
		SDL_DestroyTexture(iter->second);
	}
	SDL_DestroyRenderer(renderer_);
	SDL_DestroyWindow(window_);
	SDL_DestroySurface(window_icon_);
}

int Graphics::Initialize(Options& options)
{
	if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0)
	{
		Logger::PrintError(
			"SDL video subsystem could not initialize: " + std::string(SDL_GetError()),
			__FILE__, __LINE__);
		return -1;
	}
	if (TTF_Init() < 0)
	{
		Logger::PrintError("SDL_TTF could not initialize: " + std::string(TTF_GetError()),
			__FILE__, __LINE__);
		return -1;
	}

	// Main window
	std::string title = "SMB3DO - v" + std::string(META_VERSION);
	window_ = SDL_CreateWindow(
		title.c_str(),
		options.windowed_resolution_desired.first,
		options.windowed_resolution_desired.second,
		SDL_WINDOW_HIGH_PIXEL_DENSITY);
	if (window_ == nullptr)
	{
		Logger::PrintError("Main window could not be created: "
			+ std::string(SDL_GetError()),
			__FILE__, __LINE__);
		return -1;
	}
	window_icon_ = CreateSurfaceFromImage("data/icons/128x128/head.png");
	if (SDL_SetWindowIcon(window_, window_icon_) < 0)
	{
		Logger::PrintWarning("Could not set window icon");
	}
	
	renderer_ = SDL_CreateRenderer(window_, "opengl");
	if (renderer_ == nullptr)
	{
		Logger::PrintError("Main renderer could not be created: "
			+ std::string(SDL_GetError()),
			__FILE__, __LINE__);
		return -1;
	}
	if (options.enable_vsync)
	{
		SDL_SetRenderVSync(renderer_, 1);
	}

	// Allow for colored rect alpha transparency
	SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_BLEND);

	UpdateCanvas(options);
	UpdateViewport(options);

	if (CreateDefaultTexture() < 0)
	{
		Logger::PrintError("Could not build default texture",
			__FILE__, __LINE__);
		return -1;
	}

	return 0;
}

int Graphics::WindowToggleFullscreen(Options& options) {
	if (!is_fullscreen_)
	{
		SDL_SetWindowSize(window_, options.fullscreen_resolution_desired.first,
			options.fullscreen_resolution_desired.second);
		SDL_SyncWindow(window_);
		if (SDL_SetWindowFullscreen(window_, SDL_TRUE) < 0)
		{
			Logger::PrintError("Could not switch to fullscreen mode: "
				+ std::string(SDL_GetError()),
			__FILE__, __LINE__);
			return -1;
		}
	}
	else
	{
		if (SDL_SetWindowFullscreen(window_, SDL_FALSE) < 0)
		{
			Logger::PrintError("Could not switch to windowed mode: "
				+ std::string(SDL_GetError()),
			__FILE__, __LINE__);
			return -1;
		}
		SDL_SyncWindow(window_);
		SDL_SetWindowSize(window_, options.windowed_resolution_desired.first,
			options.windowed_resolution_desired.second);
	}
	is_fullscreen_ = !is_fullscreen_;
	return 0;
}

void Graphics::WindowSetTitle(const std::string& subtitle)
{
	std::string title = "SMB3DO - v" + std::string(META_VERSION);
	SDL_SetWindowTitle(window_, (title + " | " + subtitle).c_str());
}

int Graphics::SetViewport(const SDL_Rect* rect)
{
	if (rect == nullptr)
	{
		SDL_Rect window_rect = {0, 0, current_resolution_.first, current_resolution_.second};
		return SDL_SetRenderViewport(renderer_, &window_rect);
	}
	return SDL_SetRenderViewport(renderer_, rect);
}

const SDL_FRect& Graphics::GetViewport()
{
	return viewport_;
}

void Graphics::UpdateViewport(Options& options)
{
	int window_width, window_height;
	SDL_GetWindowSizeInPixels(window_, &window_width, &window_height);
	current_resolution_ = { window_width, window_height };
	viewport_ratio_ = options.GetViewportRatioFromPixelRatio(options.pixel_ratio);
	viewport_scaler_ = GetWindowFitViewportScaler(options);
	if (!options.enable_widescreen)
	{
		viewport_ = {
			round((current_resolution_.first
				- (NES_WINDOW_WIDTH * viewport_scaler_.first)) / 2),
			round((current_resolution_.second
				- (NES_WINDOW_HEIGHT * viewport_scaler_.second)) / 2),
			round(NES_WINDOW_WIDTH * viewport_scaler_.first),
			round(NES_WINDOW_HEIGHT * viewport_scaler_.second)};
	}
	else
	{
		viewport_ = {
			0.0,
			0.0,
			static_cast<float>(current_resolution_.first),
			static_cast<float>(current_resolution_.second)};
	}
}

std::pair<float, float> Graphics::GetWindowFitViewportScaler(Options& options, SDL_Rect viewport)
{
	float x_stretch = ((float)viewport.h * viewport_ratio_.first)
		/ ((float)viewport.w * viewport_ratio_.second);
	float y_scale = (current_resolution_.second / viewport.h);
	if (options.forceIntegerScaling)
	{
		y_scale = floor(y_scale);
	}
	float x_scale = y_scale * x_stretch;
	return std::pair<float, float>(x_scale, y_scale);
}

void Graphics::UpdateCanvas(Options& options)
{
	SDL_DestroyTexture(render_canvas_);
	if (options.enable_widescreen)
	{
		float extended_width = NES_WINDOW_HEIGHT
			* (current_resolution_.first/ current_resolution_.second)
			* (8.0 / 7.0) * ((float)viewport_ratio_.second / (float)viewport_ratio_.first);
		render_canvas_ = SDL_CreateTexture(renderer_, SDL_PIXELFORMAT_RGBA32,
			SDL_TEXTUREACCESS_TARGET, extended_width, NES_WINDOW_HEIGHT);
	}
	else
	{
		render_canvas_ = SDL_CreateTexture(renderer_, SDL_PIXELFORMAT_RGBA32,
			SDL_TEXTUREACCESS_TARGET, NES_WINDOW_WIDTH, NES_WINDOW_HEIGHT);
	}
	SDL_SetTextureScaleMode(render_canvas_, SDL_SCALEMODE_NEAREST);
	SDL_SetRenderTarget(renderer_, render_canvas_);
}

SDL_Rect Graphics::GetCanvasDimensions()
{
	float width, height;
	SDL_GetTextureSize(render_canvas_, &width, &height);
	return {0, 0, static_cast<int>(width), static_cast<int>(height)};
}

SDL_Texture* Graphics::LoadDefaultTexture()
{
	return textures_[""];
}

SDL_Texture* Graphics::LoadTextureFromImage(const std::filesystem::path& image_path)
{
	if (textures_.count(image_path.generic_string()) == 0)
	{
		SDL_Texture* texture = IMG_LoadTexture(renderer_, image_path.generic_string().c_str());
		textures_[image_path.generic_string()] = texture;
	}
	return textures_[image_path.generic_string()];
}

SDL_Texture* Graphics::LoadTextureFromImage(const std::filesystem::path& image_path,
	SDL_Color mask_color)
{
	if (textures_.count(image_path.generic_string()) == 0)
	{
		SDL_Surface* surface = CreateSurfaceFromImage(image_path);
		Uint32 color_key = SDL_MapRGB(SDL_GetPixelFormatDetails(surface->format), nullptr,
			mask_color.r, mask_color.g, mask_color.b);
		SDL_SetSurfaceColorKey(surface, SDL_TRUE, color_key);
		SDL_Texture* texture = ConvertSurfaceToTexture(surface);
		textures_[image_path.generic_string()] = texture;
	}
	return textures_[image_path.generic_string()];
}

SDL_Texture* Graphics::LoadTextureFromImage(const std::filesystem::path& image_path,
	SDL_Point mask_pixel)
{
	if (textures_.count(image_path.generic_string()) == 0)
	{
		SDL_Surface* surface = CreateSurfaceFromImage(image_path);
		Uint32 color_key = 0x00000000;
		if (mask_pixel.x >= 0 && mask_pixel.y >= 0) {
			Uint32 color_key = GetSurfacePixel(surface, mask_pixel);
			SDL_SetSurfaceColorKey(surface, SDL_TRUE, color_key);
		}
		SDL_Texture* texture = ConvertSurfaceToTexture(surface);
		textures_[image_path.generic_string()] = texture;
	}
	return textures_[image_path.generic_string()];
}

int Graphics::UnloadTexture(SDL_Texture* texture)
{
	for (std::pair<std::string, SDL_Texture*> entry : textures_) {
		if (entry.second == texture) {
			SDL_DestroyTexture(entry.second);
			textures_.erase(entry.first);
			return 0;
		}
	}
	Logger::PrintWarning("Could not unload texture at " + Logger::PointerToString(texture)
		+ ": texture not found in texture cache");
	return -1;
}

int Graphics::UnloadTexture(const std::filesystem::path& image_path)
{
	try
	{
		SDL_Texture* texture = textures_.at(image_path);
		return 0;
	}
	catch(const std::out_of_range& e)
	{
		Logger::PrintWarning("Could not unload texture for image '"
			+ image_path.generic_string() + "': image not found in texture cache");
	}
	return -1;
}

int Graphics::DrawColoredRect(const SDL_FRect* frect, SDL_Color color)
{
	SDL_SetRenderDrawColor(renderer_, color.r, color.g, color.b, color.a);
	return SDL_RenderFillRect(renderer_, frect);
}

int Graphics::DrawColoredRect(const Rectangle& rectangle, SDL_Color color)
{
	SDL_FRect sdl_frect = {rectangle.x, rectangle.y, rectangle.w, rectangle.h};
	SDL_SetRenderDrawColor(renderer_, color.r, color.g, color.b, color.a);
	return SDL_RenderFillRect(renderer_, &sdl_frect);
}

int Graphics::DrawColoredLine(const SDL_FPoint& start, const SDL_FPoint& end, SDL_Color color)
{
	SDL_SetRenderDrawColor(renderer_, color.r, color.g, color.b, color.a);
	return SDL_RenderLine(renderer_, start.x, start.y, end.x, end.y);
}

int Graphics::DrawTexture(SDL_Texture* texture, const SDL_FRect* source,
	const SDL_FRect* destination, const SDL_FlipMode flip)
{
	return SDL_RenderTextureRotated(renderer_, texture, source, destination, 0.0, nullptr, flip);
}

BitmapFont* Graphics::LoadBitmapFont(const std::filesystem::path& image_path,
	SDL_Rect glyph_dimensions, const std::string& font_name)
{
	std::string font_index = font_name;
	if (font_index.empty()) {
		font_index = image_path.generic_string();
	}
	if (bitmap_fonts_.count(font_index) == 0) {
		BitmapFont font = BitmapFont(*this, image_path, glyph_dimensions.w, glyph_dimensions.h);
		bitmap_fonts_[font_index] = std::make_unique<BitmapFont>(font);
	}
	return bitmap_fonts_[font_index].get();
}

int Graphics::SetTextFont(const std::string& font_name)
{
	if (bitmap_fonts_.count(font_name) == 0) {
		Logger::PrintError("Cannot set font to '"
			+ font_name + "'; bitmap font not loaded"),
			__FILE__, __LINE__;
		return -1;
	}
	active_bitmap_font_ = bitmap_fonts_[font_name].get();
	return 0;
}

int Graphics::DrawText(const std::string& text, SDL_Point position)
{
	if (active_bitmap_font_ == nullptr) {
		Logger::PrintError("Cannot draw text; active bitmap font is null",
			__FILE__, __LINE__);
		return -1;
	}
	return active_bitmap_font_->DrawText(*this, text, position);
	return 0;
}

void Graphics::PresentRender()
{
	// Set render target to main window
	SDL_SetRenderTarget(renderer_, nullptr);
	DrawColoredRect(nullptr, SDL_COLOR_BLACK);
	// Copy canvas to new render target
	SDL_RenderTexture(renderer_, render_canvas_, nullptr, &viewport_);
	// Present render
	SDL_RenderPresent(renderer_);
	// Set render target back to canvas
	SDL_SetRenderTarget(renderer_, render_canvas_);
}

Uint32 Graphics::GetSurfacePixel(SDL_Surface* surface, SDL_Point pixel)
{
	SDL_LockSurface(surface);
	int bpp = SDL_GetPixelFormatDetails(surface->format)->bytes_per_pixel;
	// Get address of the pixel we want to retrieve
	Uint8* ptr = (Uint8*)surface->pixels + pixel.y * surface->pitch + pixel.x * bpp;
	Uint32 pixel_value = 0x00000000;
	switch (bpp)
	{
	case 1:
		pixel_value = *ptr;
		break;
	case 2:
		pixel_value = *(Uint16*)ptr;
		break;
	case 3:
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
		{
			pixel_value =  ptr[0] << 16 | ptr[1] << 8 | ptr[2];
		}
		else
		{
			pixel_value = ptr[0] | ptr[1] << 8 | ptr[2] << 16;
		}
		break;
	case 4:
		pixel_value = *(Uint32*)ptr;
		break;
	default:
		break;
	}
	SDL_UnlockSurface(surface);
	return pixel_value;
}

int Graphics::CreateDefaultTexture()
{
	SDL_Surface* missingno_surface = IMG_ReadXPMFromArray(missingno_xpm);
	if (missingno_surface == nullptr)
	{
		Logger::PrintError("Could not load default surface from XPM; "
			+ std::string(SDL_GetError()),
			__FILE__, __LINE__);
		SDL_DestroySurface(missingno_surface);
		return -1;
	}
	SDL_Texture* default_texture = ConvertSurfaceToTexture(missingno_surface);
	if (default_texture == nullptr)
	{
		Logger::PrintError("Could not convert default surface to texture",
			__FILE__, __LINE__);
		return -1;
	}
	if (SDL_SetTextureScaleMode(default_texture, SDL_SCALEMODE_NEAREST) < 0)
	{
		Logger::PrintWarning("Could not set default texture scale mode; "
			+ std::string(SDL_GetError()));
	}
	textures_[""] = default_texture;
	return 0;
}

SDL_Texture* Graphics::ConvertSurfaceToTexture(SDL_Surface* surface)
{
	if (surface == nullptr)
	{
		Logger::PrintError("Cannot convert null surface to texture",
			__FILE__, __LINE__);
		return nullptr;
	}
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, surface);
	if (texture == nullptr)
	{
		Logger::PrintError("Could not create texture from surface at "
			+ Logger::PointerToString(surface) + "; " + std::string(SDL_GetError()),
			__FILE__, __LINE__);
	}
	SDL_DestroySurface(surface);
	return texture;
}

SDL_Surface* Graphics::CreateSurfaceFromImage(const std::filesystem::path& image_path)
{
		// TODO: load images other than just bitmaps
		SDL_Surface* surface = IMG_Load(image_path.generic_string().c_str());
		if (surface == nullptr)
		{
			Logger::PrintError("Could not create surface from image '"
				+ image_path.generic_string() + "'; " + SDL_GetError(),
			__FILE__, __LINE__);
		}
		return surface;
}