#pragma once

#include <map>
#include "sprite.h"
#include "tile.h"

/// @brief Tileset class that stores some more info than the standard Sprite, for use with Tilemaps
///        Uses a Sprite as a base image and can optionally constructed from a Tiled .tsx file.
class Tileset {
public:
	/// @brief Constructs an empty Tileset object
	Tileset();
	/// @brief Constructs a Tileset object using a Tiled .tsx file as a base
	/// @param graphics Graphics rendering context to use.
	/// @param path_to_tsx_file Path tp the .tsx file to load the tileset from
	Tileset(Graphics& graphics, const std::string& path_to_tsx_file);
	/// @brief Constructs a Tileset object using a Sprite as a base
	/// @param tileset_sprite Base tilset Sprite
	/// @param tile_width Width of a tile, in pixels, when treating the sprite as a tileset. Defaults to 16.
	/// @param tile_height Height of a tile, in pixels, when treating the sprite as a tileset. Defaults to 16.
	/// @param tile_margin Gap between the image edge and the tiles, in pixels. Defaults to 0.
	/// @param tile_spacing Gap between tiles, in pixels. Defaults to 0.
	Tileset(
		Sprite* tileset_sprite,
		int tile_width = 16, int tile_height = 16,
		int tile_margin = 0, int tile_spacing = 0
	);
	~Tileset();

	void Update(int elapsed_time_ms);
	int Draw(Graphics& graphics, int pos_x, int pos_y, unsigned int tile_id);

private:
	Sprite* tileset_sprite_;
	std::map<unsigned int, Tile> tiles_;
	int tile_width_, tile_height_;
	int tile_margin_, tile_spacing_;
	int tile_row_size_, tile_count_;

	SDL_Rect TileIndexToRect(unsigned int tile_id);
};