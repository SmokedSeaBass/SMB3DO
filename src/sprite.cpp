#include "sprite.h"

Sprite::Sprite(Graphics& graphics, const std::string& file_path, int source_x, int source_y, int width, int height) {
	source_rect_ = { source_x, source_y, width, height };
	SDL_Surface* sprite_surface = SDL_LoadBMP(file_path.c_str());
	/*SDL_Surface* sprite_surface = SDL_CreateRGBSurface(
		bmp_surface->flags,
		width, height,
		32,
		0, 0, 0, 0
	);
	SDL_BlitSurface(bmp_surface, &source_rect_, sprite_surface, NULL);
	SDL_FreeSurface(bmp_surface);*/
	texture_ = graphics.CreateTextureFromSurface(sprite_surface);  // TODO: truncate texture so that it only contains the sprite, not the whole BMP image
	SDL_FreeSurface(sprite_surface);
}

Sprite::~Sprite() {
	SDL_DestroyTexture(texture_);
}

int Sprite::Draw(Graphics& graphics, int pos_x, int pos_y) {
	SDL_Rect dest_rect = { pos_x, pos_y, source_rect_.w, source_rect_.h };
	return graphics.BlitTexture(texture_, &source_rect_, &dest_rect);
}
