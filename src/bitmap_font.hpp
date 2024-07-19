#pragma once

#include "graphics.hpp"

class Graphics;

class BitmapFont {
public:
	BitmapFont();
	BitmapFont(Graphics& graphics, const std::string& path_to_bmp, float glyph_width, float glyph_height);

	int LoadBitmap(Graphics& graphics, const std::string& path_to_bmp, float glyph_width, float glyph_height);
	int LoadBitmap(SDL_Texture* texture, float glyph_width, float glyph_height);
	int DrawText(Graphics& graphics, const std::string& text, int pos_x, int pos_y) const;

private:
	SDL_Texture* texture_;
	float texture_width_;
	float glyph_width_, glyph_height_;

	SDL_FRect GetGlyphSourceRect(char chr) const;
};