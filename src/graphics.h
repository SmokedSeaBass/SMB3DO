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
#include <memory>
#include <string>
#include "bitmap_font.h"
#include "constants.h"
#include "options.h"
#include "rectangle.h"

class BitmapFont;

/// @brief Handles rendering of objects, window/resolution changes and properties, and framerate.
class Graphics {
public:
	Graphics();
	~Graphics();

	int Initialize(Options& options);

	int WindowToggleFullscreen(Options& options);
	void WindowSetTitle(const std::string& subtitle);
	SDL_DisplayMode GetCurrentDisplayMode();
	// Set viewport SDL_Rect in screen dimension
	int SetViewport(const SDL_Rect* rect);
	const SDL_Rect& GetViewport();
	void UpdateViewport(Options& options);
	std::pair<float, float> GetWindowFitViewportScaler(Options& options, SDL_Rect viewport = { 0, 0, (int)NES_WINDOW_WIDTH, (int)NES_WINDOW_HEIGHT });
	void UpdateCanvas(Options& options);
	SDL_Rect GetCanvasDimensions();

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
	/// @brief Caches/recalls a cached SDL_Texture corresponding to a source BMP image, using a RGB value for transparency.
	/// @param file_path Path to a BMP image.
	/// @param red Red byte of transparent RGB value
	/// @param green Green byte of transparent RGB value
	/// @param blue Blue byte of transparent RGB value
	/// @return A pointer to an SDL_Texture on success, or nullptr on error.
	SDL_Texture* LoadTextureFromImage(const std::string& file_path, Uint8 red, Uint8 green, Uint8 blue);
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

	int LoadBMPFont(const std::string& path_to_bmp, unsigned int glyph_width, unsigned int glyph_height, const std::string& font_name = "");
	int SetTextFont(const std::string& font_name = "");
	int DrawText(const std::string& text, int pos_x, int pos_y);
	
	/// @brief Copies the render canvas to the main window, and then presents the render to the screen
	void PresentRender();

private:
	SDL_Window* window_main_;
	SDL_Renderer* renderer_main_;
	SDL_Texture* render_canvas_;
	SDL_Rect viewport_rect_;
	std::pair<unsigned short, unsigned short> viewport_ratio_;
	std::pair<float, float> viewport_scaler_;

	typedef std::map<std::string, SDL_Texture*> TextureCache;
	TextureCache textures_;
	typedef std::map<std::string, std::unique_ptr<BitmapFont>> BMPFontCache;
	BMPFontCache bitmap_fonts_;
	BitmapFont* active_bitmap_font_;
	
	bool is_fullscreen_;
	std::pair<float, float> current_resolution_;
};