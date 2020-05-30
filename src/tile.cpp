#include "tile.h"

Tile::Tile(int id, Sprite* tileset, SDL_Rect sprite_rect, std::string name = "NULL") {
	id_ = id;
	if (name == "NULL") {
		name_ = "TILE_" + std::to_string(id_);
	} else {
		name_ = name;
	}
	tileset_ = tileset;
	strip_rect_ = { 0, 0, 0, 0 };
	sprite_rect_ = sprite_rect;
	frame_time_ = 1;
}

Tile::~Tile() {

}

int Tile::Draw(Graphics& graphics, int pos_x, int pos_y) {
	return tileset_->Draw(graphics, pos_x, pos_y, sprite_rect_);
}