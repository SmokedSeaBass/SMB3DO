#pragma once
#include <string>
#include <list>
#include "constants.h"
#include "entity.h"
#include "tilemap.h"

// TODO 5-17-21: Decompose into sublevels.  Most definitions/members here will be translated to the sublevel class and leave the level class rather bare.

/// <summary>
/// A class that defines a single level.  A level can consist of one or more sublevels.
/// </summary>
class Level {
public:
	Level();
	~Level();

	std::string GetDisplayName();
	void SetDisplayName(std::string name);


protected:
	/// <summary>
	/// The cosmetic name for the level (e.g "Fuzzy Fields", "W 1-1")
	/// </summary>
	std::string display_name_;
	// TODO 5-19-21: Probably should be translated to the sublevel class, identify/access any sublevel via 0xWWLLAAAA. Maybe include entrances?: 0xWWLLAAEE
	/// <summary>
	/// The level's unique 32-bit identifier number. Intended practice is that the format is 0xWWWWLLLL,
	/// where WWWW is the 16-bit world number and LLLL is the 16-bit level number.
	/// </summary>
	Uint32 uid_;
	unsigned int timelimit_;
	SDL_Rect bounds_;
	std::list<Entity> entity_list_;
	std::list<Sprite> tileset_list_;
	std::list<Tilemap> tilemap_layers_;
};
