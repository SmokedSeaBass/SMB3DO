#include "tileset.h"

#include <memory>
#include <stdexcept>
#include <vector>
#include "tinyxml2\tinyxml2.h"
#include "error.h"

Tileset::Tileset() :
	tileset_sprite_(nullptr),
	tile_width_(0),
	tile_height_(0),
	tile_margin_(0),
	tile_spacing_(0),
	tile_row_size_(0),
	tile_count_(0) {
}

Tileset::Tileset(Graphics& graphics, const std::string& path_to_tsx_file) : Tileset::Tileset() {

	tinyxml2::XMLDocument tmx;
	if (tmx.LoadFile(path_to_tsx_file.c_str()) != tinyxml2::XML_SUCCESS) {
		Error::PrintError("Could not load TSX file '" + path_to_tsx_file + "'");
		return;
	}

	tinyxml2::XMLElement* tileset_node = tmx.FirstChildElement("tileset");
	if (tileset_node == nullptr) { return; }
	tile_width_ = tileset_node->FindAttribute("tilewidth")->IntValue();
	tile_height_ = tileset_node->FindAttribute("tileheight")->IntValue();
	tile_count_ = tileset_node->FindAttribute("tilecount")->IntValue();
	tile_row_size_ = tileset_node->FindAttribute("columns")->IntValue();
	const tinyxml2::XMLAttribute* margin_attribute = tileset_node->FindAttribute("margin");
	if (margin_attribute != nullptr) {
		tile_margin_ = margin_attribute->IntValue();
	}
	const tinyxml2::XMLAttribute* spacing_attribute = tileset_node->FindAttribute("spacing");
	if (spacing_attribute != nullptr) {
		tile_spacing_ = spacing_attribute->IntValue();
	}

	tinyxml2::XMLElement* image_node = tileset_node->FirstChildElement("image");
	std::string source_image_path = path_to_tsx_file + "/../" + image_node->FindAttribute("source")->Value();
	tileset_sprite_ = new Sprite(graphics, source_image_path);

	// TODO 6-7-21: Iterate through tile nodes to create Tile objects and store them in the tileset's Tiles table
	// Properties to extract from each node include collision type, hitbox, animation, etc.
	tinyxml2::XMLElement* tile_node = image_node->NextSiblingElement("tile");
	while (tile_node != nullptr) {
		unsigned int tile_id = tile_node->FindAttribute("id")->IntValue();
		SDL_Rect tile_rect = Tileset::TileIndexToRect(tile_id);

		/* Object/tile collisions */
		Tile::COLLISION_TYPE tile_collision = Tile::COLLISION_TYPE::NONE;
		tinyxml2::XMLElement* objgrp_node = tile_node->FirstChildElement("objectgroup");
		if (objgrp_node != nullptr) {
			tinyxml2::XMLElement* obj_node = objgrp_node->FirstChildElement("object");
			while (obj_node != nullptr) {
				std::string obj_type = obj_node->FindAttribute("type")->Value();
				if (obj_type == "SolidCollision") {
					tile_collision = Tile::COLLISION_TYPE::SOLID;
				}
				obj_node = obj_node->NextSiblingElement("object");
			}
		}

		/* Animation */
		AnimatedSprite* tile_sprite = nullptr;
		tinyxml2::XMLElement* anim_node = tile_node->FirstChildElement("animation");
		std::vector<std::pair<unsigned int, unsigned int>> animation_tiles;
		if (anim_node != nullptr) {
			tinyxml2::XMLElement* frame_node = anim_node->FirstChildElement("frame");
			while (frame_node != nullptr) {
				unsigned int frame_tile_id = frame_node->FindAttribute("tileid")->IntValue();
				unsigned int frame_length_ms = frame_node->FindAttribute("duration")->IntValue();
				std::pair<unsigned int, unsigned int> frame_pair = { frame_tile_id, frame_length_ms };
				animation_tiles.push_back(frame_pair);
				frame_node = frame_node->NextSiblingElement("frame");
			}
			double frame_speed = 1000.0 / animation_tiles[0].second;
			int frame_count = animation_tiles.size();
			tile_sprite = new AnimatedSprite(graphics, tileset_sprite_->GetTexture(), tile_rect.x, tile_rect.y, tile_rect.w, tile_rect.h, frame_speed, frame_count, tile_spacing_);
		}

		/* Storing the tile */
		if (tile_collision != Tile::COLLISION_TYPE::NONE || tile_sprite != nullptr) {		// Is the tile worth storing?
			Tile tile = Tile(tile_id, tile_sprite, tile_collision);
			tiles_[tile_id] = tile;
		}

		tile_node = tile_node->NextSiblingElement("tile");
	}
}

Tileset::Tileset(Sprite* tileset_sprite, int tile_width, int tile_height, int tile_margin, int tile_spacing) : Tileset::Tileset() {
	tileset_sprite_ = tileset_sprite;
	tile_width_ = tile_width;
	tile_height_ = tile_height;
	tile_margin_ = tile_margin;
	tile_spacing_ = tile_spacing;
	double sprite_width = tileset_sprite_->GetTextureWidth();
	tile_row_size_ = static_cast<int>((sprite_width - tile_margin_ + tile_spacing_) / (tile_width_ + tile_spacing_));
	if (tile_row_size_ < 0) {
		tile_row_size_ = 0;
	}
	double sprite_height = tileset_sprite_->GetTextureHeight();
	int tile_col_size = (int)ceil((sprite_height - tile_margin_) / (tile_height_ + tile_spacing_)) - 1;
	tile_count_ = tile_row_size_ * tile_col_size;
}

Tileset::~Tileset() {
	for (TileList::iterator iter = tiles_.begin(); iter != tiles_.end(); ++iter) {
		delete iter->second.GetSprite();
	}
	delete tileset_sprite_;
}

Sprite* Tileset::GetTilesetSprite() {
	return tileset_sprite_;
}

Tile Tileset::GetTileFromId(unsigned int tile_id) {
	//std::map<unsigned int, Tile>::iterator iter;
	//iter = tiles_.find(tile_id);
	//if (iter != tiles_.end()) {
	//	return iter->second;
	//} else {
	//	return Tile(tile_id);
	//}
	if (tiles_.count(tile_id) == 0) {
		return Tile(tile_id);
	}
	return tiles_[tile_id];
}

void Tileset::Update(double delta_time) {
	for (std::pair<unsigned int, Tile> tile_pair : tiles_) {
		tile_pair.second.Update(delta_time);
	}
}

int Tileset::Draw(Graphics& graphics, int pos_x, int pos_y, unsigned int tile_id) {
	if (tiles_.count(tile_id) != 0) {
		Tile tile = tiles_[tile_id];
		if (tile.GetSprite() != nullptr) {
			return tile.Draw(graphics, pos_x, pos_y);
		}
	}
	SDL_Rect tile_rect = TileIndexToRect(tile_id);
	return tileset_sprite_->Draw(graphics, pos_x, pos_y, tile_rect);
}


SDL_Rect Tileset::TileIndexToRect(unsigned int tile_id) {
	int row_index = tile_id % tile_row_size_;
	int col_index = tile_id / tile_row_size_;
	int rect_x = row_index * (tile_width_ + tile_spacing_) + tile_margin_;
	int rect_y = col_index * (tile_height_ + tile_spacing_) + tile_margin_;
	SDL_Rect tile_rect = {
		rect_x, rect_y, tile_width_, tile_height_
	};
	return tile_rect;
}