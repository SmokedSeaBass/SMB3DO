#pragma once

#include "graphics.hpp"

class Graphics;

class BitmapFont
{
public:
	BitmapFont();
	BitmapFont(
		Graphics& graphics,
		std::filesystem::path image_path,
		float glyph_width,
		float glyph_height);

	int LoadBitmap(Graphics& graphics, std::filesystem::path image_path,
		float glyph_width, float glyph_height);
	int LoadBitmap(SDL_Texture* texture, float glyph_width, float glyph_height);
	int DrawText(Graphics& graphics, const std::string& text, SDL_Point position) const;

private:
	SDL_Texture* texture_;
	float texture_width_;
	float glyph_width_, glyph_height_;

	SDL_FRect GetGlyphSourceRect(char chr) const;
};