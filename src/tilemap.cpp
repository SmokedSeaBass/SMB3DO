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


void Tilemap::GetDimensions(int dimensions[]) {
	dimensions[0] = width_;
	dimensions[1] = height_;
}

Tile Tilemap::GetTile(int x, int y) const {
	unsigned int tile_id = GetTileId(x, y);
	if (tile_id == UINT_MAX) {
		return Tile(tile_id);
	}
	unsigned int tile_index = tile_id;
	const Tileset* tileset = GetTileset(tile_id, &tile_index);
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

const Tileset* Tilemap::GetTileset(unsigned int tile_id, unsigned int* tile_index) const {
	unsigned int counter = 0;
	for (std::shared_ptr<Tileset> tileset : tilesets_) {
		tile_id -= counter;
		counter += tileset->GetTileCount();
		if (tile_id < counter) {
			if (tile_index != nullptr) {
				*tile_index = tile_id;
			}
			return tileset.get();
		}
	}
	return nullptr;
}

std::vector<Tilemap::CollisionTile> Tilemap::GetCollidingTiles(const Rectangle& rect) const {
	std::vector<CollisionTile> colliding_tiles;
	// TODO 6-21-21: Account for offset tilemap/tilemap position
	Rectangle collision_rect = rect + Rectangle(pos_x_, pos_y_, 0, 0);
	int top_row = static_cast<int>(floor(rect.Top() / TILESIZE_NES));
	int bottom_row = static_cast<int>(ceil(rect.Bottom() / TILESIZE_NES) - 1);
	int left_col = static_cast<int>(floor(rect.Left() / TILESIZE_NES));
	int right_col = static_cast<int>(ceil(rect.Right() / TILESIZE_NES) - 1);
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

void Tilemap::Update(double delta_time) {
	for (std::shared_ptr<Tileset> tileset : tilesets_) {
		tileset->Update(delta_time);
	}
}

int Tilemap::Draw(Graphics& graphics, int offset_x, int offset_y, Rectangle crop) {
	if (crop.w < 0) crop.w = graphics.GetViewport().w;
	if (crop.h < 0) crop.h = graphics.GetViewport().h;
	crop.x -= offset_x;
	crop.y -= offset_y;

	unsigned int tile_id = 0;
	int top_row = std::max(static_cast<int>(floor(crop.Top() / TILESIZE_NES)), 0);
	int bottom_row = std::min(static_cast<int>(floor(crop.Bottom() / TILESIZE_NES)), static_cast<int>(height_) + 1);
	int left_col = std::max(static_cast<int>(floor(crop.Left() / TILESIZE_NES)), 0);
	int right_col = std::min(static_cast<int>(floor(crop.Right() / TILESIZE_NES)), static_cast<int>(width_) + 1);
	for (int y = top_row; y <= bottom_row; y++) {
		for (int x = left_col; x <= right_col; x++) {
			unsigned int tile_id = GetTileId(x, y);
			if (tile_id == UINT_MAX) {
				continue;
			}
			unsigned int tile_index = tile_id;
			const Tileset* tileset = GetTileset(tile_id, &tile_index);
			if (tileset == nullptr) {
				SDL_Rect dest_rect = { x * TILESIZE_NES + pos_x_ + offset_x, y * TILESIZE_NES + pos_y_ + offset_y, TILESIZE_NES , TILESIZE_NES };
				graphics.DrawTexture(graphics.GetDefaultTexture(), nullptr, &dest_rect);
			} else {
				tileset->Draw(graphics, x * TILESIZE_NES + pos_x_ + offset_x, y * TILESIZE_NES + pos_y_ + offset_y, tile_index);
			}
		}
	}
	return 0;
}