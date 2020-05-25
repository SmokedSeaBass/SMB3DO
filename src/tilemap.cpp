#include "tilemap.h"

#include <stdio.h>
#include <stdexcept>
#include "constants.h"

Tilemap::Tilemap(unsigned char map[], unsigned int width, unsigned int height) {
	width_ = width;
	height_ = height;
	map_ = new unsigned char[width_ * height_];
	memcpy(map_, map, sizeof(char) * (width_ * height_));
}

Tilemap::~Tilemap() {
	delete[] map_;
}

void Tilemap::GetDimensions(int dimensions[]) {
	dimensions[0] = width_;
	dimensions[1] = height_;
}

unsigned char Tilemap::GetTileId(int x, int y) {
	if (x >= width_ || y >= height_) {
		throw std::out_of_range("ERROR: Attempted to get tile outside of tilemap");
	}
	return map_[y * width_ + x];
}

unsigned char Tilemap::SetTileId(int x, int y, unsigned char tile_id) {
	if (x >= width_ || y >= height_) {
		throw std::out_of_range("ERROR: Attempted to set tile outside of tilemap");
	}
	map_[y * width_ + x] = tile_id;
	return map_[y * width_ + x];
}

int Tilemap::Draw(Graphics& graphics, int pos_x, int pos_y) {
	SDL_Rect block_rect = { 0, 0, BLOCKSIZE_NES, BLOCKSIZE_NES };
	SDL_Color color = { 0x00, 0x00, 0x00, 0xFF };
	// TODO: Optimize rendering such that only visible tiles are drawn
	for (int y = 0; y < height_; y++) {
		block_rect.y = pos_y + y * BLOCKSIZE_NES;
		for (int x = 0; x < width_; x++) {
			block_rect.x = pos_x + x * BLOCKSIZE_NES;
			switch (map_[y * width_ + x]) {
			case 0x00:
				continue;
			case 0x01:
				color = SDL_COLOR_WHITE;
				break;
			case 0x02:
				color = SDL_COLOR_BLACK;
				break;
			default:
				continue;
			}
			graphics.BlitColoredRect(&block_rect, color.r, color.g, color.b, color.a);
		}
	}
	return 0;
}