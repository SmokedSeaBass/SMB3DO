#include "tileset.h"

Tileset::Tileset() {
	tileset_sprite_ = nullptr;
	tile_width_ = 0;
	tile_height_ = 0;
	tile_margin_ = 0;
	tile_spacing_ = 0;
	tile_row_size_ = 0;
	tile_count_ = 0;
}

Tileset::Tileset(const std::string& path_to_tsx_file, int tile_width, int tile_height, int tile_margin, int tile_spacing) {
	// TODO 6-5-21
}



Tileset::Tileset(Sprite* tileset_sprite, int tile_width, int tile_height, int tile_margin, int tile_spacing) {
	tileset_sprite_ = tileset_sprite;
	tile_width_ = tile_width;
	tile_height_ = tile_height;
	tile_margin_ = tile_margin;
	tile_spacing_ = tile_spacing;
	double sprite_width = tileset_sprite_->GetTextureWidth();
	tile_row_size_ = static_cast<int>((sprite_width - tile_margin_ + tile_spacing_) / (tile_width_ + tile_spacing_));
	if (tile_row_size_ < 0) {
		tile_row_size_ = 0;
	}
	double sprite_height = tileset_sprite_->GetTextureHeight();
	int tile_col_size = (int)ceil((sprite_height - tile_margin_) / (tile_height_ + tile_spacing_)) - 1;
	tile_count_ = tile_row_size_ * tile_col_size;
}

Tileset::~Tileset() { }

int Tileset::Draw(Graphics& graphics, int pos_x, int pos_y, int tile_index)
{
	int row_index = tile_index % tile_row_size_;
	int col_index = tile_index / tile_row_size_;
 	int rect_x = row_index * (tile_width_ + tile_spacing_);
	int rect_y = col_index * (tile_height_ + tile_spacing_);
	SDL_Rect tile_rect = {
		rect_x, rect_y, tile_width_, tile_height_
	};
	return tileset_sprite_->Draw(graphics, pos_x, pos_y, tile_rect);
}