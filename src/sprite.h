#pragma once

#include <string>
#include "graphics.h"

/// <summary>
/// Stores a sprite sheet image and can draw a rectangular section of it.  Stores a sprite sheet(or any image really) as a SDL_Texture.
/// Contains a gettable / settable SDL_Rect that specifies what part of theSDL_Texture to draw when Draw() is called.
/// </summary>
class Sprite {
public:
	enum class ORIGIN_ORIENTATION {
		CENTER,
		TOP_LEFT,
		TOP_MIDDLE,
		TOP_RIGHT,
		MIDDLE_LEFT,
		MIDDLE_RIGHT,
		BOTTOM_LEFT,
		BOTTOM_MIDDLE,
		BOTTOM_RIGHT
	};

	/// <summary>
	/// Constructs a Sprite class.
	/// </summary>
	/// <param name="graphics">Graphics rendering context to use.</param>
	/// <param name="file_path">File path to the BMP image to load for the Sprite.</param>
	/// <param name="alpha_x">X-coordinate in pixels of the pixel of the loaded image whose RGB value to treat as transparent.  Defaults to -1 (no transparency).</param>
	/// <param name="alpha_y">Y-coordinate in pixels of the pixel of the loaded image whose RGB value to treat as transparent.  Defaults to -1 (no transparency).</param>
	/// <param name="source_x">Initial x-coordinate of the rect used for rendering the sprite.  Defaults to -1 (will not render).</param>
	/// <param name="source_y">Initial y-coordinate of the rect used for rendering the sprite.  Defaults to -1 (will not render).</param>
	/// <param name="source_w">Initial width of the rect used for rendering the sprite.  Defaults to -1 (will not render).</param>
	/// <param name="source_h">Initial height of the rect used for rendering the sprite.  Defaults to -1 (will not render).</param>
	/// <returns></returns>
	Sprite(Graphics& graphics, const std::string& file_path, int alpha_x = -1, int alpha_y = -1, int source_x = -1, int source_y = -1, int source_w = -1, int source_h = -1);
	Sprite(Graphics& graphics, SDL_Texture* texture, int source_x = -1, int source_y = -1, int source_w = -1, int source_h = -1);
	~Sprite();

	SDL_Rect GetRect();
	void SetRect(SDL_Rect& rect);
	void SetRect(int rect_x, int rect_y, int rect_w, int rect_h);

	void SetOrigin(int x, int y);
	void SetOrigin(ORIGIN_ORIENTATION origin_orientation);

	SDL_Texture* GetTexture();
	int GetTextureHeight();
	int GetTextureWidth();

	int Draw(Graphics& graphics, int pos_x, int pos_y);
	int Draw(Graphics& graphics, int pos_x, int pos_y, SDL_Rect alt_source_rect);  // Does NOT modify source_rect_ member

private:
	SDL_Texture* texture_;
	SDL_Rect source_rect_;
	double origin_x_, origin_y_;
	int texture_width_, texture_height_;
	Uint32 color_key_;  // Save if we want to change later?
};