#include "sprite.h"

#include <stdexcept>
#include "error.h"
#include "graphics/missingno.xpm"

Sprite::Sprite(Graphics& graphics, const std::string& file_path, int alpha_x, int alpha_y, int source_x, int source_y, int source_w, int source_h) {
	source_rect_ = { source_x, source_y, source_w, source_h };
	origin_x_ = 0.0;
	origin_y_ = 0.0;
	color_key_ = 0x00000000;
	SDL_Surface* sprite_surface = SDL_LoadBMP(file_path.c_str());
	if (sprite_surface == NULL) {
		SDL_FreeSurface(sprite_surface);
		std::string error_msg = "Could not find file: '" + file_path + "'";
		Error::PrintError(error_msg);
		// Attempt to use embedded fallback sprite
		texture_ = graphics.GetDefaultTexture();
		source_rect_ = { 0, 0, 16, 16 };
		SDL_QueryTexture(texture_, nullptr, nullptr, &texture_width_, &texture_height_);
		return;
	}
	texture_width_ = sprite_surface->w;
	texture_height_ = sprite_surface->h;
	// Set transparency based on RGB of given pixel
	if (alpha_x >= 0 && alpha_y >= 0) {
		Uint8 r = 0x00, g = 0x00, b = 0x00;
		Uint32 alpha_pixel = graphics.GetSurfacePixel(sprite_surface, alpha_x, alpha_y);
		SDL_GetRGB(alpha_pixel, sprite_surface->format, &r, &g, &b);
		color_key_ = SDL_MapRGB(sprite_surface->format, r, g, b);
		SDL_SetColorKey(sprite_surface, SDL_TRUE, color_key_);
	}
	texture_ = graphics.CreateTextureFromSurface(sprite_surface);
	SDL_FreeSurface(sprite_surface);  // Perhaps store surface too if we later want to adjust the texture?
}

Sprite::Sprite(Graphics& graphics, SDL_Texture* texture, int source_x, int source_y, int source_w, int source_h) {
	source_rect_ = { source_x, source_y, source_w, source_h };
	origin_x_ = 0.0;
	origin_y_ = 0.0;
	if (texture != nullptr) {
		texture_ = texture;
	} else {
		std::string err = "Cannot create sprite from null texture";
		Error::PrintError(err);
		texture_ = graphics.GetDefaultTexture();
	}
	SDL_QueryTexture(texture_, nullptr, nullptr, &texture_width_, &texture_height_);
	color_key_ = 0x00000000;		// unknown, maybe pullable from SDL_Texture?
}

Sprite::~Sprite() {
	SDL_DestroyTexture(texture_);
}

SDL_Rect Sprite::GetRect() {
	return source_rect_;
}

void Sprite::SetRect(SDL_Rect& rect) {
	source_rect_.x = rect.x;
	source_rect_.y = rect.y;
	source_rect_.w = rect.w;
	source_rect_.h = rect.h;
}

void Sprite::SetRect(int rect_x, int rect_y, int rect_w, int rect_h) {
	source_rect_.x = rect_x;
	source_rect_.y = rect_y;
	source_rect_.w = rect_w;
	source_rect_.h = rect_h;
}

void Sprite::SetOrigin(int x, int y) {
	origin_x_ = x;
	origin_y_ = y;
}

void Sprite::SetOrigin(Sprite::ORIGIN_ORIENTATION origin_orientation) {
	// TODO: Make ORIGIN_ORIENTATION bitwise, for optimized calculations
	if (origin_orientation == Sprite::ORIGIN_ORIENTATION::TOP_LEFT ||
		origin_orientation == Sprite::ORIGIN_ORIENTATION::TOP_MIDDLE ||
		origin_orientation == Sprite::ORIGIN_ORIENTATION::TOP_RIGHT) {
		origin_y_ = 0.0;
		if (origin_orientation == Sprite::ORIGIN_ORIENTATION::TOP_LEFT) origin_x_ = 0.0;
		if (origin_orientation == Sprite::ORIGIN_ORIENTATION::TOP_MIDDLE) origin_x_ = source_rect_.w / 2.0;
		if (origin_orientation == Sprite::ORIGIN_ORIENTATION::TOP_RIGHT) origin_x_ = source_rect_.w;
		return;
	}
	if (origin_orientation == Sprite::ORIGIN_ORIENTATION::MIDDLE_LEFT ||
		origin_orientation == Sprite::ORIGIN_ORIENTATION::CENTER ||
		origin_orientation == Sprite::ORIGIN_ORIENTATION::MIDDLE_RIGHT) {
		origin_y_ = source_rect_.h / 2.0;
		if (origin_orientation == Sprite::ORIGIN_ORIENTATION::MIDDLE_LEFT) origin_x_ = 0.0;
		if (origin_orientation == Sprite::ORIGIN_ORIENTATION::CENTER) origin_x_ = source_rect_.w / 2.0;
		if (origin_orientation == Sprite::ORIGIN_ORIENTATION::MIDDLE_RIGHT) origin_x_ = source_rect_.w;
		return;
	}
	if (origin_orientation == Sprite::ORIGIN_ORIENTATION::BOTTOM_LEFT ||
		origin_orientation == Sprite::ORIGIN_ORIENTATION::BOTTOM_MIDDLE ||
		origin_orientation == Sprite::ORIGIN_ORIENTATION::BOTTOM_RIGHT) {
		origin_y_ = source_rect_.h;
		if (origin_orientation == Sprite::ORIGIN_ORIENTATION::BOTTOM_LEFT) origin_x_ = 0.0;
		if (origin_orientation == Sprite::ORIGIN_ORIENTATION::BOTTOM_MIDDLE) origin_x_ = source_rect_.w / 2.0;
		if (origin_orientation == Sprite::ORIGIN_ORIENTATION::BOTTOM_RIGHT) origin_x_ = source_rect_.w;
		return;
	}
}

SDL_Texture* Sprite::GetTexture() {
	return texture_;
}

int Sprite::GetTextureHeight() {
	return texture_height_;
}

int Sprite::GetTextureWidth() {
	return texture_width_;
}

int Sprite::Draw(Graphics& graphics, int pos_x, int pos_y) {
	SDL_Rect dest_rect = {
		static_cast<int>(round(pos_x - origin_x_)),
		static_cast<int>(round(pos_y - origin_y_)),
		source_rect_.w,
		source_rect_.h
	};
	return graphics.BlitTexture(texture_, &source_rect_, &dest_rect);
}

int Sprite::Draw(Graphics& graphics, int pos_x, int pos_y, SDL_Rect alt_source_rect) {
	SDL_Rect dest_rect = {
		static_cast<int>(round(pos_x - origin_x_)),
		static_cast<int>(round(pos_y - origin_y_)),
		alt_source_rect.w,
		alt_source_rect.h
	};
	return graphics.BlitTexture(texture_, &alt_source_rect, &dest_rect);
}

