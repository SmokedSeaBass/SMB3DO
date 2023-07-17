#pragma once

#include <filesystem>
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
	/// @param path_to_tsx_file Path to the .tsx file to load the tileset from
	Tileset(Graphics& graphics, std::filesystem::path path_to_tsx_file);
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

	unsigned int GetTileCount() const;

	const Sprite* GetTilesetSprite() const;
	const Tile GetTileFromTileIndex(unsigned int tile_index) const;

	void Update(double delta_time);
	int Draw(Graphics& graphics, int pos_x, int pos_y, unsigned int tile_index) const;

private:
	/// @brief Used for rendering tiles that do not have an associated Tile object
	std::shared_ptr<Sprite> tileset_sprite_;
	typedef std::map<unsigned int, std::shared_ptr<Tile>> TileList;
	TileList tiles_;
	int tile_width_, tile_height_;
	int tile_margin_, tile_spacing_;
	int tile_row_size_, tile_count_;

	SDL_Rect TileIndexToRect(unsigned int tile_id) const;
};