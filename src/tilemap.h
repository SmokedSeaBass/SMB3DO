#pragma once
#include <vector>
#include "graphics.h"
#include "tile.h"
#include "sprite.h"
#include "tileset.h"

///	<summary>
/// Handles the tile grid routines.
/// </summary>
/// <remarks>
///	A Tilemap is a data structure that organizes tiles into a 2-D grid.  Each entry in the tilemap's map is
///	an unsigned char corresponding to a unique tile, which has its own properties, interaction subroutines, and drawing routine.
///	These properties are defined in the Tile class.
/// </remarks>
class Tilemap {
public:
	Tilemap();
	Tilemap(std::vector<std::vector<unsigned int>> map, Tileset* tileset);
	Tilemap(std::string path_to_tmx);
	~Tilemap();

	void GetDimensions(int dimensions[]);
	unsigned int GetTileId(int x, int y);
	void SetTileId(int x, int y, unsigned int tile_id);
	void SetTileset(Tileset* tileset);
	Tileset* GetTileset();
	int Draw(Graphics& graphics, int pos_x, int pos_y);

private:
	Tileset* tileset_;		// TODO 6-5-21: Replace with an array of pointers to support multiple tilesets
	unsigned int width_;
	unsigned int height_;
	std::vector<std::vector<unsigned int>> map_;
};