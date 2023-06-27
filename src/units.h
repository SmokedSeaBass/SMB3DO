#pragma once

namespace Units {
	/// @brief Screen pixel units, will vary from Game units depending on output window resolution
	typedef int Pixels;
	/// @brief Game units (one NES pixel is one Game unit)
	typedef double Game;
	/// @brief Tile units (16 Game units are one Tile unit)
	typedef int Tile;

};