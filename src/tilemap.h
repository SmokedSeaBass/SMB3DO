#pragma once
#include <vector>
#include "graphics.h"
#include "tile.h"

/**
*  \brief Handles the tile grid routines.
* 
*  \description A Tilemap is a data structure that organizes tiles into a 2-D grid.  Each entry in the tilemap's map is
*               an unsigned char corresponding to a unique tile, which has it's own properties, interaction routine, and drawing routine.
*               These properties are defined in the Tile class.
*/
class Tilemap {
public:
	Tilemap(Tile* map[], unsigned int width, unsigned int height);
	~Tilemap();

	void GetDimensions(int dimensions[]);
	Tile* GetTile(int x, int y);
	void SetTile(int x, int y, Tile* tile);
	int Draw(Graphics& graphics, int pos_x, int pos_y);

private:
	unsigned int width_;
	unsigned int height_;
	Tile** map_;
};