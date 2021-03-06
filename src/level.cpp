#include "level.h"

Level::Level() {
	display_name_ = std::string("Test Level");
	timelimit_ = 999;
	bounds_ = {0, 0, 0, 0};
	entity_list_ = std::vector<Entity>();
	tileset_list_ = std::vector<Sprite>();
	tilemap_layers_ = std::vector<Tilemap>();
}

Level::~Level() { }

std::string Level::GetDisplayName() {
	return display_name_;
}

void Level::SetDisplayName(std::string display_name) {
	display_name_ = display_name;
}
