#pragma once
#include <vector>
#include "graphics.h"
#include "rectangle.h"
#include "sprite.h"
#include "tile.h"
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
	Tilemap(std::vector<std::vector<unsigned int>> tilemap, Tileset* tileset);
	Tilemap(Graphics& graphics, std::string path_to_tmx);
	~Tilemap();

	void GetDimensions(int dimensions[]);
	Tile GetTile(int x, int y);
	unsigned int GetTileId(int x, int y);
	void SetTileId(int x, int y, unsigned int tile_id);
	void SetTileset(Tileset* tileset);
	Tileset* GetTileset();
	std::vector<Tile> GetCollidingTiles(const Rectangle& rect);
	int Draw(Graphics& graphics, int offset_x, int offset_y, Rectangle crop = { 0, 0, -1, -1 } );

private:
	Tileset* tileset_;		// TODO 6-5-21: Replace with an array of pointers to support multiple tilesets
	unsigned int width_, height_;
	int pos_x_, pos_y_;
	std::vector<std::vector<unsigned int>> tilemap_;
};