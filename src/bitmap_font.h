#pragma once

#include "sprite.h"

class BitmapFont {
public:
	BitmapFont();
	BitmapFont(Graphics& graphics, const std::string& path_to_bmp, int glyph_width, int glyph_height);
	~BitmapFont();

	int LoadBitmap(Graphics& graphics, const std::string& path_to_bmp, int glyph_width, int glyph_height);
	int LoadBitmap(SDL_Texture* texture, int glyph_width, int glyph_height);
	SDL_Rect GetGlyphSourceRect(char chr) const;
	int DrawText(Graphics& graphics, const std::string& text, int pos_x, int pos_y) const;

private:
	SDL_Texture* texture_;
	int texture_width_;
	int glyph_width_, glyph_height_;
};