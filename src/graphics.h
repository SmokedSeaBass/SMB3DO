#pragma once

#if defined(_WIN32 )
	#include <SDL.h>
	#include <SDL_image.h>
	#include <SDL_ttf.h>
#elif __linux__
	#include <SDL2/SDL.h>
	#include <SDL2/SDL_image.h>
	#include <SDL2/SDL_ttf.h>
#endif
#include <map>
#include <string>
#include "constants.h"
#include "options.h"
#include "rectangle.h"

/// @brief Handles rendering of objects, window/resolution changes and properties, and framerate.
class Graphics {
public:
	Graphics();
	~Graphics();

	int Initialize(Options& options);

	int WindowToggleFullscreen(Options& options);
	void WindowSetTitle(const std::string& subtitle);
	SDL_DisplayMode GetCurrentDisplayMode();
	int SetViewport(const SDL_Rect* rect);
	const SDL_Rect& GetViewport();
	void UpdateViewport(Options& options);
	std::pair<float, float> GetWindowFitViewportScaler(Options& options, SDL_Rect viewport = { 0, 0, WINDOW_WIDTH_NES, WINDOW_HEIGHT_NES });

	int BuildDefaultTexture();
	SDL_Texture* GetDefaultTexture();
	/// @brief Creates an SDL_Texture from an SDL_Surface, and then frees the SDL_Surface.
	/// @param surface Pointer to an SDL_Surface.
	/// @return Pointer to an SDL_Texture on success, or NULL on error.
	SDL_Texture* CreateTextureFromSurface(SDL_Surface* surface);
	/// @brief Caches/recalls a cached SDL_Texture corresponding to a source BMP image.
	/// @param file_path Path to a BMP image.
	/// @return A pointer to an SDL_Texture on success, or nullptr on error.
	SDL_Texture* LoadTextureFromImage(const std::string& file_path);
	/// @brief Caches/recalls a cached SDL_Texture corresponding to a source BMP image, utilizing a 32-bit color key.
	/// @param file_path Path to a BMP image.
	/// @param color_key 32-bit color key.
	/// @return A pointer to an SDL_Texture on success, or nullptr on error.
	SDL_Texture* LoadTextureFromImage(const std::string& file_path, Uint32 color_key);
	/// @brief Caches/recalls a cached SDL_Texture corresponding to a source BMP image, utilizing a specific pixel for transparency.
	/// @param file_path Path to a BMP image.
	/// @param alpha_x Integer x-coordinate of the color mask pixel.
	/// @param alpha_y Integer y-coordinate of the color mask pixel.
	/// @return A pointer to an SDL_Texture on success, or nullptr on error.
	SDL_Texture* LoadTextureFromImage(const std::string& file_path, int alpha_x, int alpha_y);
	/// @brief Unloads an SDL_Texture from the texture cache.
	/// @param texture Pointer to an SDL_Texture.
	/// @return 0 on success, or -1 if the texture is not cached.
	int UnloadTexture(SDL_Texture* texture);
	Uint32 GetSurfacePixel(SDL_Surface* surface, int x, int y);
	int DrawColoredRect(const SDL_Rect* rect, Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha);
	int DrawColoredRect(const Rectangle& rect, Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha);
	int DrawColoredLine(const std::pair<int, int>& point_1, const std::pair<int, int>& point_2, Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha);
	int DrawColoredOutline(const std::pair<int, int>& point_1, const std::pair<int, int>& point_2, Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha);
	int DrawColoredOutline(const Rectangle& rect, Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha);
	int DrawTexture(SDL_Texture* texture, const SDL_Rect* source_rect, const SDL_Rect* dest_rect, const SDL_RendererFlip flip = SDL_FLIP_NONE);
	int FlipRenderer();

private:
	SDL_Window* window_main_;
	SDL_Renderer* renderer_main_;
	typedef std::map<std::string, SDL_Texture*> TextureMap;
	TextureMap texture_cache_;
	
	bool is_fullscreen_;
	std::pair<float, float> current_resolution_;
	SDL_Rect viewport_rect_;
	std::pair<float, float> viewport_scaler_;
	std::pair<unsigned short, unsigned short> viewport_ratio_;
};