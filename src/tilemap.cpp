#include "tilemap.h"

#include <stdexcept>
#include <sstream>
#include "tinyxml2.h"
#include "error.h"
#include "constants.h"

Tilemap::Tilemap() {
	map_ = std::vector<std::vector<unsigned int>>();
	tileset_ = nullptr;
	width_ = 0;
	height_ = 0;
}

Tilemap::Tilemap(std::vector<std::vector<unsigned int>> map, Tileset* tileset) {
	map_ = map;
	height_ = map.size();
	width_ = map[0].size();
	tileset_ = tileset;
}

Tilemap::Tilemap(std::string path_to_tmx) {
	width_ = 0;
	height_ = 0;
	tileset_ = nullptr;		// TODO 6-6-21: Exctract tileset/.tsx from
	
	// Get tile id csv data from .tmx file
	tinyxml2::XMLDocument tmx;
	if (tmx.LoadFile(path_to_tmx.c_str()) != tinyxml2::XML_SUCCESS) {
		Error::PrintError("Could not load TMX file '" + path_to_tmx + "'");
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
	map_ = map;
}

Tilemap::~Tilemap() { }


void Tilemap::GetDimensions(int dimensions[]) {
	dimensions[0] = width_;
	dimensions[1] = height_;
}

unsigned int Tilemap::GetTileId(int x, int y) {
	if (x >= width_ || y >= height_) {
		throw std::out_of_range("Attempted to get tile outside of tilemap");
	}
	return map_[y][x];
}

void Tilemap::SetTileId(int x, int y, unsigned int tile_id) {
	if (x >= width_ || y >= height_) {
		throw std::out_of_range("Attempted to set tile outside of tilemap");
	}
	map_[y][x] = tile_id;
}

void Tilemap::SetTileset(Tileset* tileset) {
	tileset_ = tileset;
}

Tileset* Tilemap::GetTileset() {
	return tileset_;
}

int Tilemap::Draw(Graphics& graphics, int pos_x, int pos_y) {
	unsigned int tile_id = 0;
	// TODO: Optimize rendering such that only visible tiles are drawn
	for (int y = 0; y < height_; y++) {
		for (int x = 0; x < width_; x++) {
			tile_id = map_[y][x];
			if (tile_id == UINT_MAX) {
				continue;
			}
			if (tileset_ != nullptr) {
				tileset_->Draw(graphics, x * TILESIZE_NES + pos_x, y * TILESIZE_NES + pos_y, tile_id);
			} else {
				SDL_Rect dest_rect = { x * TILESIZE_NES + pos_x, y * TILESIZE_NES + pos_y, TILESIZE_NES , TILESIZE_NES };
				graphics.BlitTexture(graphics.GetDefaultTexture(), nullptr, &dest_rect);
			}
			
		}
	}
	return 0;
}