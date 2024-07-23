#pragma once

#include <filesystem>
#include <map>
#include <memory>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>
#include "bitmap_font.hpp"
#include "constants.hpp"
#include "options.hpp"
#include "rectangle.hpp"

class BitmapFont;

/// @brief Handles rendering of objects, window/resolution changes and properties, and framerate.
class Graphics
{
public:
	Graphics();
	~Graphics();

	int Initialize(Options& options);

	int WindowToggleFullscreen(Options& options);
	void WindowSetTitle(const std::string& subtitle);
	// Set viewport SDL_Rect in screen dimension
	int SetViewport(const SDL_Rect* rect);
	const SDL_FRect& GetViewport();
	void UpdateViewport(Options& options);
	std::pair<float, float> GetWindowFitViewportScaler(
		Options& options,
		SDL_Rect viewport = {
			0,
			0,
			static_cast<int>(NES_WINDOW_WIDTH),
			static_cast<int>(NES_WINDOW_HEIGHT)});
	void UpdateCanvas(Options& options);
	SDL_Rect GetCanvasDimensions();

	SDL_Texture* LoadDefaultTexture();

	/// @brief Loads the texture of an image.
	/// @param image_path the filesystem path to the image.
	/// @return a pointer to an SDL_Texture of the image on success, or nullptr on error.
	SDL_Texture* LoadTextureFromImage(const std::filesystem::path& image_path);

	/// @brief Loads the texture of an image, using an RGB value for transparency.
	/// @param file_path the filesystem path to the image.
	/// @param mask_color the color to treat as transparent (alpha channel is ignored).
	/// @return A pointer to an SDL_Texture on success, or nullptr on error.
	SDL_Texture* LoadTextureFromImage(
		const std::filesystem::path& image_path,
		SDL_Color mask_color);
	
	/// @brief Loads the texture of an image, using a given pixel's color for transparency.
	/// @param file_path the filesystem path to the image.
	/// @param mask_pixel the pixel whose color should be treated as
	/// transparent.
	/// @return a pointer to an SDL_Texture on success, or nullptr on error.
	SDL_Texture* LoadTextureFromImage(
		const std::filesystem::path& image_path,
		SDL_Point mask_pixel);

	/// @brief Unloads an SDL_Texture from the texture cache.
	/// @param texture the pointer to the SDL_Texture.
	/// @return 0 on success, or -1 if the texture does not exist in the cache.
	int UnloadTexture(SDL_Texture* texture);

	/// @brief Unloads an image from the texture cache.
	/// @param image_path the filesystem path to the image.
	/// @return 0 on success, or -1 if the image texture does not exist in the cache.
	int UnloadTexture(const std::filesystem::path& image_path);

	int DrawColoredRect(const SDL_FRect* frect, SDL_Color color);
	int DrawColoredRect(const Rectangle& rectangle, SDL_Color color);
	int DrawColoredLine(const SDL_FPoint& start, const SDL_FPoint& end, SDL_Color color);
	int DrawTexture(
		SDL_Texture* texture,
		const SDL_FRect* source,
		const SDL_FRect* destination,
		const SDL_FlipMode flip = SDL_FLIP_NONE);

	BitmapFont* LoadBitmapFont(const std::filesystem::path& image_path,
		SDL_Rect glyph_dimensions, const std::string& font_name = "");
	int SetTextFont(const std::string& font_name = "");
	int DrawText(const std::string& text, SDL_Point position);
	
	/// @brief Copies the render canvas to the main window,
	/// and then presents the render to the screen
	void PresentRender();

private:
	SDL_Window* window_;
	SDL_Surface* window_icon_;
	SDL_Renderer* renderer_;
	SDL_Texture* render_canvas_;
	SDL_FRect viewport_;
	std::pair<unsigned int, unsigned int> viewport_ratio_;
	std::pair<float, float> viewport_scaler_;

	typedef std::map<std::string, SDL_Texture*> TextureCache;
	TextureCache textures_;
	typedef std::map<std::string, std::unique_ptr<BitmapFont>> BitmapFontCache;
	BitmapFontCache bitmap_fonts_;
	BitmapFont* active_bitmap_font_;
	
	bool is_fullscreen_;
	std::pair<unsigned int, unsigned int> current_resolution_;

	static inline char* missingno_xpm[] = {
		"16 16 3 1",
		" 	c #FF00D8",
		".	c #000000",
		"+	c #FFFFFF",
		"      ....      ",
		"    ..++++..    ",
		"   .++++++++.   ",
		"  .++++++++++.  ",
		"  .+++...++++.  ",
		"  .++..  .+++.  ",
		"   ...   .+++.  ",
		"        .+++..  ",
		"       .+++..   ",
		"      .+++..    ",
		"      .++..     ",
		"       ...      ",
		"      ...       ",
		"     .+++.      ",
		"     .+++.      ",
		"      ....      "};

	Uint32 GetSurfacePixel(SDL_Surface* surface, SDL_Point pixel);

	int CreateDefaultTexture();

	/// @brief Creates an SDL_Surface from an image
	/// @param image_path the filesystem path to the image
	/// @return a pointer to an SDL_Surface on success, or nullptr on failure.
	SDL_Surface* CreateSurfaceFromImage(const std::filesystem::path& image_path);
	
	/// @brief Creates an SDL_Texture from an SDL_Surface and then destroys the SDL_Surface.
	/// @param surface a pointer to the SDL_Surface.
	/// @return a pointer to an SDL_Texture on success, or nullptr on failure.
	SDL_Texture* ConvertSurfaceToTexture(SDL_Surface* surface);
};