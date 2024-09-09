#pragma once

#include <filesystem>
#include <map>
#include "sprite.hpp"
#include "tile.hpp"

/// @brief Tileset class that stores some more info than the standard Sprite, for use with Tilemaps
///        Uses a Sprite as a base image and can optionally constructed from a Tiled .tsx file.
class Tileset {
public:
	typedef unsigned int TileId;

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
	
	/// @brief Gets the tile from the tileset associated with the give tile ID.
	/// @param tile_id 
	/// @return A pointer to the tile object
	const Tile* GetTileFromTileId(TileId tile_id) const;

	void Update(double delta_time);
	int Draw(Graphics& graphics, int pos_x, int pos_y, unsigned int tile_index) const;

private:
	/// @brief Used for rendering tiles that do not have an associated Tile object
	Sprite* tileset_sprite_;
	std::map<TileId, Tile> tiles_;
	Units::Pixels tile_width_, tile_height_;
	Units::Pixels tile_margin_, tile_spacing_;
	unsigned int tile_row_size_, tile_count_;

	/// @brief Gives the clip rectangle of the tileset sprite for the given tile.
	/// @param tile_id The ID of the tile to get the clip for.
	/// @return A rectangle defining the spritesheet region for the tile.
	SDL_FRect GetClipFromTileId(TileId tile_id) const;
};