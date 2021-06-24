#pragma once

#include <string>
#include "graphics.h"

/// <summary>
/// Stores a sprite sheet image and can draw a rectangular section of it.  Stores a sprite sheet (or any image really) as a SDL_Texture.
/// Contains a gettable / settable SDL_Rect that specifies what part of theS DL_Texture to draw when Draw() is called.
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

	Sprite();
	/// @brief Constructs a Sprite object using a BMP image source
	/// @param graphics Graphics rendering context to use.
	/// @param file_path File path to the BMP source image.
	/// @param alpha_x X-coordinate of the pixel of the loaded image whose RGB value to treat as transparent. Defaults to -1 (no transparency).
	/// @param alpha_y Y-coordinate of the pixel of the loaded image whose RGB value to treat as transparent. Defaults to -1 (no transparency).
	/// @param source_x Initial x-coordinate of the rect used for rendering the sprite. Defaults to -1 (will not render).
	/// @param source_y Initial y-coordinate of the rect used for rendering the sprite. Defaults to -1 (will not render).
	/// @param source_w Initial width of the rect used for rendering the sprite. Defaults to -1 (will not render).
	/// @param source_h Initial height of the rect used for rendering the sprite. Defaults to -1 (will not render).
	Sprite(
		Graphics& graphics,
		const std::string& file_path,
		int alpha_x = -1, int alpha_y = -1,
		int source_x = -1, int source_y = -1, int source_w = -1, int source_h = -1
	);
	/// @brief Constructs a Sprite class using an SDL_Texture* source
	/// @param graphics Graphics rendering context to use.
	/// @param texture Pointer to an SDL_Texture to use as the sprite's source
	/// @param source_x Initial x-coordinate of the rect used for rendering the sprite. Defaults to -1 (will not render).
	/// @param source_y Initial y-coordinate of the rect used for rendering the sprite. Defaults to -1 (will not render).
	/// @param source_w Initial width of the rect used for rendering the sprite. Defaults to -1 (will not render).
	/// @param source_h Initial height of the rect used for rendering the sprite. Defaults to -1 (will not render).
	Sprite(
		Graphics& graphics,
		SDL_Texture* texture,
		int source_x = -1, int source_y = -1, int source_w = -1, int source_h = -1
	);
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

protected:
	SDL_Rect source_rect_;

private:
	SDL_Texture* texture_;
	double origin_x_, origin_y_;
	Uint32 color_key_;  // Save if we want to change later?
};
