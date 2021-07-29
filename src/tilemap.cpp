#include "tilemap.h"

#include <stdexcept>
#include <sstream>
#include "tinyxml2\tinyxml2.h"
#include "error.h"
#include "constants.h"
#include "tileset.h"

Tilemap::Tilemap() : 
	tilemap_(std::vector<std::vector<unsigned int>>()),
	width_(0),
	height_(0),
	pos_x_(0),
	pos_y_(0) {
	background_color_ = { 0, 0, 0, 0 };
}

Tilemap::Tilemap(std::vector<std::vector<unsigned int>> tilemap, Tileset* tileset) : Tilemap::Tilemap() {
	tilemap_ = tilemap;
	if (tileset == nullptr) {
		tilesets_.push_back(nullptr);
	} else {
		tilesets_.push_back(std::make_shared<Tileset>(*tileset));
	}
	width_ = tilemap[0].size();
	height_ = tilemap.size();
}

Tilemap::Tilemap(Graphics& graphics, std::string path_to_tmx) : Tilemap::Tilemap() {
	// Get tile id csv data from .tmx
	tinyxml2::XMLDocument tmx;
	if (tmx.LoadFile(path_to_tmx.c_str()) != tinyxml2::XML_SUCCESS) {
		Error::PrintError("Could not load TMX file: '" + path_to_tmx + "'");
		return;
	}
	tinyxml2::XMLElement* map_node = tmx.FirstChildElement("map");
	const tinyxml2::XMLAttribute* backgroundcolor_attr = map_node->FindAttribute("backgroundcolor");
	if (backgroundcolor_attr != nullptr) {
		const std::string backgroundcolor = backgroundcolor_attr->Value();
		background_color_.r = std::stoul(backgroundcolor.substr(1, 2), nullptr, 16);
		background_color_.g = std::stoul(backgroundcolor.substr(3, 2), nullptr, 16);
		background_color_.b = std::stoul(backgroundcolor.substr(5, 2), nullptr, 16);
		background_color_.a = 0xFF;
	}
	tinyxml2::XMLElement* layer_node = map_node->FirstChildElement("layer");
	width_ = layer_node->FindAttribute("width")->IntValue();
	height_ = layer_node->FindAttribute("height")->IntValue();
	tinyxml2::XMLElement* data_node = layer_node->FirstChildElement("data");
	std::string tile_id_data = data_node->GetText();

	// Format and read tile id csv data
	tile_id_data = tile_id_data.substr(1, std::string::npos);		// Remove leading newline
	std::vector<std::vector<unsigned int>> map;
	std::stringstream data_stream(tile_id_data);
	std::string line;
	while (std::getline(data_stream, line, '\n')) {
		std::vector<unsigned int> row;
		std::stringstream line_stream(line);
		std::string tile_id;
		while (std::getline(line_stream, tile_id, ',')) {
			row.push_back(atoi(tile_id.c_str()) - 1);
		}
		map.push_back(row);
	}
	tilemap_ = map;

	// Get tileset from .tmx
	tinyxml2::XMLElement* tileset_node = map_node->FirstChildElement("tileset");
	std::string path_to_tsx = "";
	while (tileset_node != nullptr) {
		path_to_tsx = path_to_tmx + "/../" + tileset_node->FindAttribute("source")->Value();
		Tileset tileset = Tileset(graphics, path_to_tsx);
		if (tileset.GetTilesetSprite() == nullptr) {
			Error::PrintWarning("Tileset '" + path_to_tsx + "' extracted from TMX file '" + path_to_tmx +  "'  has null Sprite");
		}
		tilesets_.push_back(std::make_shared<Tileset>(tileset));
		tileset_node = tileset_node->NextSiblingElement("tileset");
	}
}


std::vector<int> Tilemap::GetDimensions() const {
	return { static_cast<int>(width_), static_cast<int>(height_) };
}

SDL_Color Tilemap::GetBackgroundColor() const {
	return background_color_;
}

Tile Tilemap::GetTile(int x, int y) const {
	unsigned int tile_id = GetTileId(x, y);
	if (tile_id == UINT_MAX) {
		return Tile(tile_id);
	}
	unsigned int tile_index = tile_id;
	const Tileset* tileset = GetTilesetFromTileID(tile_id, &tile_index);
	if (tileset == nullptr) {
		return Tile(tile_id);
	}
	return tileset->GetTileFromTileIndex(tile_index);
}

unsigned int Tilemap::GetTileId(int x, int y) const{
	if (x >= width_ || y >= height_) {
		//throw std::out_of_range("Attempted to get tile outside of tilemap");
		return UINT_MAX;
	}
	return tilemap_[y][x];
}

const Tileset* Tilemap::GetTilesetFromTileID(unsigned int tile_id, unsigned int* tile_index) const {
	unsigned int tileset_end_tile_id = 0;
	unsigned int index = 0;
	for (std::shared_ptr<Tileset> tileset : tilesets_) {
		index = tile_id - tileset_end_tile_id;
		tileset_end_tile_id += tileset->GetTileCount();
		if (tileset_end_tile_id > tile_id) {
			if (tile_index != nullptr) {
				*tile_index = index;
			}
			return tileset.get();
		}
	}
	return nullptr;
}

std::vector<Tileset*> Tilemap::GetTilesets() const {
	std::vector<Tileset*> tilesets;
	for (std::shared_ptr<Tileset> tileset : tilesets_) {
		tilesets.push_back(tileset.get());
	}
	return tilesets;
}

std::vector<Tilemap::CollisionTile> Tilemap::GetCollidingTiles(const Rectangle& rect) const {
	std::vector<CollisionTile> colliding_tiles;
	// TODO 6-21-21: Account for offset tilemap/tilemap position
	Rectangle collision_rect = rect + Rectangle(pos_x_, pos_y_, 0, 0);
	int top_row = (int)(floor(rect.Top() / (int)TILESIZE_NES));
	int bottom_row = (int)(ceil(rect.Bottom() / (int)TILESIZE_NES) - 1);
	int left_col = (int)(floor(rect.Left() / (int)TILESIZE_NES));
	int right_col = (int)(ceil(rect.Right() / (int)TILESIZE_NES) - 1);
	for (int y = top_row; y <= bottom_row; y++) {
		for (int x = left_col; x <= right_col; x++) {
			CollisionTile col_tile;
			col_tile.tile = GetTile(x, y);
			col_tile.row = y;
			col_tile.col = x;
			colliding_tiles.push_back(col_tile);
		}
	}
	return colliding_tiles;
}

void Tilemap::SetTileId(int x, int y, unsigned int tile_id) {
	if (x >= width_ || y >= height_) {
		throw std::out_of_range("Attempted to set tile outside of tilemap");
	}
	tilemap_[y][x] = tile_id;
}

void Tilemap::AddTileset(Tileset* tileset) {
	if (tileset == nullptr) {
		return;
	}
	tilesets_.push_back(std::make_shared<Tileset>(*tileset));
}

void Tilemap::Update(double delta_time) {
	for (std::shared_ptr<Tileset> tileset : tilesets_) {
		tileset->Update(delta_time);
	}
}

int Tilemap::Draw(Graphics& graphics, int offset_x, int offset_y, Rectangle crop) {
	if (crop.w < 0) crop.w = graphics.GetCanvasDimensions().w;
	if (crop.h < 0) crop.h = graphics.GetCanvasDimensions().h;
	crop.x -= offset_x; 
	crop.y -= offset_y;

	unsigned int tile_id = 0;
	int top_row = std::max((int)(floor(crop.Top() / (int)TILESIZE_NES)), 0);
	int bottom_row = std::min((int)(floor(crop.Bottom() / (int)TILESIZE_NES)), (int)height_ + 1);
	int left_col = std::max((int)(floor(crop.Left() / (int)TILESIZE_NES)), 0);
	int right_col = std::min((int)(floor(crop.Right() / (int)TILESIZE_NES)), (int)width_ + 1);
	for (int y = top_row; y <= bottom_row; y++) {
		for (int x = left_col; x <= right_col; x++) {
			unsigned int tile_id = GetTileId(x, y);
			if (tile_id == UINT_MAX) {
				continue;
			}
			unsigned int tile_index = tile_id;
			const Tileset* tileset = GetTilesetFromTileID(tile_id, &tile_index);
			if (tileset == nullptr) {
				SDL_Rect dest_rect = { x * (int)TILESIZE_NES + pos_x_ + offset_x, y * (int)TILESIZE_NES + pos_y_ + offset_y, (int)TILESIZE_NES , (int)TILESIZE_NES };
				graphics.DrawTexture(graphics.GetDefaultTexture(), nullptr, &dest_rect);
			} else {
				tileset->Draw(graphics, x * (int)TILESIZE_NES + pos_x_ + offset_x, y * (int)TILESIZE_NES + pos_y_ + offset_y, tile_index);
			}
		}
	}
	return 0;
}