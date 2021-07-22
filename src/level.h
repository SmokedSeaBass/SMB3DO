#pragma once

#include <memory>
#include <string>
#include "camera.h"
#include "constants.h"
#include "entity.h"
#include "tileset.h"
#include "tilemap.h"

// TODO 5-17-21: Decompose into sublevels.  Most definitions/members here will be translated to the sublevel class and leave the level class rather bare.

/// <summary>
/// A class that defines a single level.  A level can consist of one or more sublevels.
/// </summary>
class Level {
public:
	Level();

	std::string GetDisplayName() const;

	void SetDisplayName(std::string name);

	int Load(Graphics& graphics, const std::string& path_to_lvl);
	void Update(Graphics& graphics, const Input& input, double delta_time);
	int Draw(Graphics& graphics);

protected:
	/// <summary>
	/// The cosmetic name for the level (e.g "Fuzzy Fields", "W 1-1")
	/// </summary>
	std::string display_name_;
	// TODO 5-19-21: Probably should be translated to the sublevel class, identify/access any sublevel via 0xWWLLAAAA. Maybe include entrances?: 0xWWLLAAEE
	/// <summary>
	/// The level's 
	///  32-bit identifier number. Intended practice is that the format is 0xWWWWLLLL,
	/// where WWWW is the 16-bit world number and LLLL is the 16-bit level number.
	/// </summary>
	Uint32 uid_;
	unsigned int timelimit_;
	SDL_Color background_color_;
	SDL_Rect bounds_;
	Camera camera_;
	std::vector<std::shared_ptr<Entity>> entity_list_;
	std::vector<Tileset*> tileset_list_;
	std::vector<Tilemap> tilemap_layers_;
};
