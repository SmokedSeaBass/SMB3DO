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
	struct CollisionTile {
		Tile tile;
		int col, row;
	};

	Tilemap();
	Tilemap(std::vector<std::vector<unsigned int>> tilemap, Tileset* tileset);
	Tilemap(Graphics& graphics, std::string path_to_tmx);

	void GetDimensions(int dimensions[]);
	Tile GetTile(int x, int y) const;
	unsigned int GetTileId(int x, int y) const;
	void SetTileId(int x, int y, unsigned int tile_id);
	void AddTileset(Tileset* tileset);
	/// @brief Returns a pointer to the Tileset that a Tile belongs to by tile ID
	/// @param tile_id The tile ID of the Tile
	/// @param tile_index (Optional) Pointer to an unsigned integer to be set to the tile's index within its found Tileset (for use with drawing)
	/// @return On success, a pointer to a Tileset. Otherwise, nullptr
	const Tileset* GetTileset(unsigned int tile_id, unsigned int* tile_index = nullptr) const;
	std::vector<CollisionTile> GetCollidingTiles(const Rectangle& rect) const;
	void Update(double delta_time);
	int Draw(Graphics& graphics, int offset_x = 0, int offset_y = 0, Rectangle crop = { 0, 0, -1, -1 } );

private:
	std::vector<std::shared_ptr<Tileset>> tilesets_;
	unsigned int width_, height_;
	int pos_x_, pos_y_;
	std::vector<std::vector<unsigned int>> tilemap_;
};