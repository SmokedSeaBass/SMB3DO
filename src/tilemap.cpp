#include "tilemap.h"

#include <stdio.h>
#include <stdexcept>
#include "constants.h"

Tilemap::Tilemap(Tile* map[], unsigned int width, unsigned int height) {
	width_ = width;
	height_ = height;
	map_ = new Tile*[width_ * height_];
	memcpy(map_, map, sizeof(Tile*) * (width_ * height_));
}

Tilemap::~Tilemap() {
	delete[] map_;
}


void Tilemap::GetDimensions(int dimensions[]) {
	dimensions[0] = width_;
	dimensions[1] = height_;
}

Tile* Tilemap::GetTile(int x, int y) {
	if (x >= width_ || y >= height_) {
		throw std::out_of_range("ERROR: Attempted to get tile outside of tilemap");
	}
	return map_[y * width_ + x];
}

void Tilemap::SetTile(int x, int y, Tile* tile) {
	if (x >= width_ || y >= height_) {
		throw std::out_of_range("ERROR: Attempted to set tile outside of tilemap");
	}
	map_[y * width_ + x] = tile;
}

int Tilemap::Draw(Graphics& graphics, int pos_x, int pos_y) {
	Tile* tile = nullptr;
	// TODO: Optimize rendering such that only visible tiles are drawn
	for (int y = 0; y < height_; y++) {
		for (int x = 0; x < width_; x++) {
			tile = map_[y * width_ + x];
			if (tile == nullptr) {
				continue;
			}
			tile->Draw(graphics, x * BLOCKSIZE_NES + pos_x, y * BLOCKSIZE_NES + pos_y);
		}
	}
	return 0;
}