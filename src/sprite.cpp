#include "sprite.h"

#include <stdexcept>
#include "error.h"
#include "graphics/missingno.xpm"

Sprite::Sprite() : 
	texture_ (nullptr),
	source_rect_({ 0, 0, 0, 0 }),
	origin_x_(0),
	origin_y_(0) {
}

Sprite::Sprite(Graphics& graphics, const std::string& file_path, int alpha_x, int alpha_y, int source_x, int source_y, int source_w, int source_h) : Sprite() {
	source_rect_ = { source_x, source_y, source_w, source_h };
	texture_ = graphics.LoadTextureFromImage(file_path, alpha_x, alpha_y);
}

Sprite::Sprite(Graphics& graphics, SDL_Texture* texture, int source_x, int source_y, int source_w, int source_h) : Sprite() {
	if (texture == nullptr) {
		Error::PrintWarning("Sprite initialized with null texture");
		return;
	}
	source_rect_ = { source_x, source_y, source_w, source_h };
	texture_ = texture;
}

Sprite::Sprite(Graphics& graphics, const std::string& file_path, SDL_Rect source_rect, Uint8 alpha_red, Uint8 alpha_green, Uint8 alpha_blue) : Sprite() {
	source_rect_ = { source_rect.x, source_rect.y, source_rect.w, source_rect.h };
	texture_ = graphics.LoadTextureFromImage(file_path, alpha_red, alpha_green, alpha_blue);
}

SDL_Rect Sprite::GetSourceRect() const {
	return source_rect_;
}

void Sprite::SetSourceRect(SDL_Rect& rect) {
	source_rect_.x = rect.x;
	source_rect_.y = rect.y;
	source_rect_.w = rect.w;
	source_rect_.h = rect.h;
}

void Sprite::SetSourceRect(int rect_x, int rect_y, int rect_w, int rect_h) {
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

SDL_Texture* Sprite::GetTexture() const {
	return texture_;
}

int Sprite::GetTextureHeight() const {
	int height;
	SDL_QueryTexture(texture_, NULL, NULL, NULL, &height);
	return height;
}

int Sprite::GetTextureWidth() const {
	int width;
	SDL_QueryTexture(texture_, NULL, NULL, &width, NULL);
	return width;
}

int Sprite::Draw(Graphics& graphics, int pos_x, int pos_y, const SDL_RendererFlip flip) const {
	SDL_Rect dest_rect = {
		static_cast<int>(round(pos_x - origin_x_)),
		static_cast<int>(round(pos_y - origin_y_)),
		source_rect_.w,
		source_rect_.h
	};
	if (texture_ == nullptr) {
		SDL_Rect default_rect = { 0, 0, 16, 16 };
		return graphics.DrawTexture(graphics.GetDefaultTexture(), &default_rect, &dest_rect, flip);
	}
	return graphics.DrawTexture(texture_, &source_rect_, &dest_rect, flip);
}

int Sprite::Draw(Graphics& graphics, int pos_x, int pos_y, SDL_Rect alt_source_rect, const SDL_RendererFlip flip) const {
	SDL_Rect dest_rect = {
		static_cast<int>(round(pos_x - origin_x_)),
		static_cast<int>(round(pos_y - origin_y_)),
		alt_source_rect.w,
		alt_source_rect.h
	};
	if (texture_ == nullptr) {
		return graphics.DrawTexture(graphics.GetDefaultTexture(), &alt_source_rect, &dest_rect, flip);
	}
	return graphics.DrawTexture(texture_, &alt_source_rect, &dest_rect, flip);
}

