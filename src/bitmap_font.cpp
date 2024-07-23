#include "bitmap_font.hpp"

#include "logger.hpp"

BitmapFont::BitmapFont() :
	texture_(nullptr),
	texture_width_(0),
	glyph_width_(0),
	glyph_height_(0){
}

BitmapFont::BitmapFont(Graphics& graphics, std::filesystem::path image_path,
	float glyph_width, float glyph_height) : BitmapFont::BitmapFont()
{
	LoadBitmap(graphics, image_path, glyph_width, glyph_height);
}

int BitmapFont::LoadBitmap(Graphics& graphics, std::filesystem::path image_path,
	float glyph_width, float glyph_height)
{
	if (texture_ != nullptr)
	{
		Logger::PrintError("Cannot load image '" + image_path.generic_string()
			+ "' for bitmap font; bitmap font already has a texture set");
		return -1;
	}
	SDL_Texture* texture = graphics.LoadTextureFromImage(image_path, SDL_Point{0, 0});
	if (texture == nullptr)
	{
		Logger::PrintError("Cannot not load null texture for bitmap font");
		return -1;
	}
	return LoadBitmap(texture, glyph_width, glyph_height);
}

int BitmapFont::LoadBitmap(SDL_Texture* texture, float glyph_width, float glyph_height)
{
	if (texture_ != nullptr)
	{
		Logger::PrintError("Cannot load texture at '" + Logger::PointerToString(texture)
			+ "' for bitmap font; bitmap font already has a texture set");
		return -1;
	}
	if (texture == nullptr)
	{
		Logger::PrintError("Cannot not load null texture for bitmap font");
		return -1;
	}
	texture_ = texture;
	SDL_GetTextureSize(texture_, &texture_width_, nullptr);
	glyph_width_ = glyph_width;
	glyph_height_ = glyph_height;
	return 0;
}

SDL_FRect BitmapFont::GetGlyphSourceRect(char chr) const
{
	if (texture_width_ == 0 || glyph_width_ == 0)
	{
		return { 0, 0, 0, 0 };
	}
	int ascii_code = static_cast<int>(chr);
	int glyph_index = ascii_code - 16 * 2;		// Remove first 32 special characters
	int row_size = texture_width_ / glyph_width_;
	int glyph_row = glyph_index / row_size;
	int glyph_col = glyph_index % row_size;
	SDL_FRect glyph_rect = {
		glyph_col * glyph_width_,
		glyph_row * glyph_height_,
		glyph_width_,
		glyph_height_ };
	return glyph_rect;
}

int BitmapFont::DrawText(Graphics& graphics, const std::string& text, SDL_Point position) const
{
	float x = position.x;
	float y = position.y;
	SDL_FRect src_frect;
	SDL_FRect dst_frect;
	for (char chr : text)
	{
		if (chr == '\n')
		{
			x = position.x;
			y += glyph_height_;
			continue;
		}
		dst_frect = {x, y, glyph_width_, glyph_height_};
		if (texture_ != nullptr)
		{
			src_frect = GetGlyphSourceRect(chr);
			graphics.DrawTexture(texture_, &src_frect, &dst_frect);
		}
		else
		{
			src_frect = {0, 0, glyph_width_, glyph_height_};
			graphics.DrawTexture(graphics.LoadDefaultTexture(), &src_frect, &dst_frect);
		}
		x += glyph_width_;
	}
	return 0;
}
