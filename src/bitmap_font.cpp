#include "bitmap_font.h"

#include "logger.h"

BitmapFont::BitmapFont() :
	texture_(nullptr),
	texture_width_(0),
	glyph_width_(0),
	glyph_height_(0){
}

BitmapFont::BitmapFont(Graphics& graphics, const std::string& path_to_bmp, int glyph_width, int glyph_height) : BitmapFont::BitmapFont() {
	LoadBitmap(graphics, path_to_bmp, glyph_width, glyph_height);
}

int BitmapFont::LoadBitmap(Graphics& graphics, const std::string& path_to_bmp, int glyph_width, int glyph_height) {
	if (texture_ != nullptr) {
		Logger::PrintError("Loading BMP \'" + path_to_bmp + "\' for BitmapFont: BMP already loaded fro BitmapFont");
		return -1;
	}
	texture_ = graphics.LoadTextureFromImage(path_to_bmp, 0, 0);
	if (texture_ == nullptr) {
		Logger::PrintError("Loading texture from BMP \'" + path_to_bmp + "\' for BitmapFont: could not load texture from image");
		return -1;
	}
	SDL_QueryTexture(texture_, NULL, NULL, &texture_width_, NULL);
	glyph_width_ = glyph_width;
	glyph_height_ = glyph_height;
	return 0;
}

int BitmapFont::LoadBitmap(SDL_Texture* texture, int glyph_width, int glyph_height) {
	if (texture_ != nullptr) {
		Logger::PrintError("Cannt load texture \'" + Logger::ptr_to_string(texture) + "\' to BitmapFont with BMP already loaded");
		return -1;
	}
	texture_ = texture;
	SDL_QueryTexture(texture_, NULL, NULL, &texture_width_, NULL);
	glyph_width_ = glyph_width;
	glyph_height_ = glyph_height;
	return 0;
}

SDL_Rect BitmapFont::GetGlyphSourceRect(char chr) const {
	if (texture_width_ == 0 || glyph_width_ == 0) {
		return { 0, 0, 0, 0 };
	}
	int ascii_code = static_cast<int>(chr);
	int glyph_index = ascii_code - 16 * 2;		// Remove first 32 special characters
	int row_size = texture_width_ / glyph_width_;
	int glyph_row = glyph_index / row_size;
	int glyph_col = glyph_index % row_size;
	SDL_Rect glyph_rect = { glyph_col * glyph_width_, glyph_row * glyph_height_, glyph_width_, glyph_height_ };
	return glyph_rect;
}

int BitmapFont::DrawText(Graphics& graphics, const std::string& text, int pos_x, int pos_y) const {
	int x = pos_x;
	int y = pos_y;
	SDL_Rect src_rect;
	SDL_Rect dst_rect;
	for (char chr : text) {
		if (chr == '\n') {
			x = pos_x;
			y += glyph_height_;
			continue;
		}
		dst_rect = { x, y, glyph_width_, glyph_height_ };
		if (texture_ != nullptr) {
			src_rect = GetGlyphSourceRect(chr);
			graphics.DrawTexture(texture_, &src_rect, &dst_rect);
		} else {
			src_rect = { 0, 0, glyph_width_, glyph_height_ };
			graphics.DrawTexture(graphics.GetDefaultTexture(), &src_rect, &dst_rect);
		}
		x += glyph_width_;
	}
	return 0;
}
