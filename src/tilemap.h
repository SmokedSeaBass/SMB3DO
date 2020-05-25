#pragma once
#include <vector>
#include "graphics.h"

class Tilemap {
public:
	Tilemap(unsigned char map[], unsigned int width, unsigned int height);
	~Tilemap();

	void GetDimensions(int dimensions[]);
	unsigned char GetTileId(int x, int y);
	unsigned char SetTileId(int x, int y, unsigned char tile_id);
	int Draw(Graphics& graphics, int pos_x, int pos_y);

private:
	unsigned int width_;
	unsigned int height_;
	unsigned char* map_;
};