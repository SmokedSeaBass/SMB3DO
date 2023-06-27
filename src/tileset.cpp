#include "tileset.h"

#include <memory>
#include <stdexcept>
#include <vector>
#include <tinyxml2/tinyxml2.h>
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
		Error::PrintError("Could not load TSX file: '" + path_to_tsx_file + "'");
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
	std::string color_key = "";
 	const tinyxml2::XMLAttribute* trans_param = image_node->FindAttribute("trans");
	if (trans_param != nullptr) {
		std::string color_key = trans_param->Value();
		unsigned int alpha_red = std::stoul(color_key.substr(0, 2), nullptr, 16);
		unsigned int alpha_green = std::stoul(color_key.substr(2, 2), nullptr, 16);
		unsigned int alpha_blue = std::stoul(color_key.substr(4, 2), nullptr, 16);
		tileset_sprite_ = std::make_shared<Sprite>(Sprite(graphics, source_image_path, { -1, -1, -1, -1 }, alpha_red, alpha_green, alpha_blue));
	} else {
		tileset_sprite_ = std::make_shared<Sprite>(Sprite(graphics, source_image_path));
	}

	// Properties to extract from each node include collision type, hitbox, animation, etc.
	tinyxml2::XMLElement* tile_node = image_node->NextSiblingElement("tile");
	while (tile_node != nullptr) {
		// Tile ID
		unsigned int tile_id = tile_node->FindAttribute("id")->IntValue();
		SDL_Rect tile_rect = TileIndexToRect(tile_id);

		// Tile/collision type
		Tile::COLLISION_TYPE tile_collision = Tile::COLLISION_TYPE::NONE;
		const tinyxml2::XMLAttribute* tile_type_attr = tile_node->FindAttribute("type");
		if (tile_type_attr != nullptr) {
			const std::string tile_type = tile_type_attr->Value();
			if (tile_type == "Solid") {
				tile_collision = Tile::COLLISION_TYPE::SOLID;
			} else if (tile_type == "Semisolid") {
				tile_collision = Tile::COLLISION_TYPE::SEMISOLID;
			} else if (tile_type == "Coin") {
				tile_collision = Tile::COLLISION_TYPE::COIN;
			} else if (tile_type == "Hurt") {
				tile_collision = Tile::COLLISION_TYPE::HURT;
			} else if (tile_type == "Kill") {
				tile_collision = Tile::COLLISION_TYPE::KILL;
			} else if (tile_type == "Hittable") {
				tile_collision = Tile::COLLISION_TYPE::HITTABLE;
			}
		}

		///* Object/tile collisions */
		//Tile::COLLISION_TYPE tile_collision = Tile::COLLISION_TYPE::NONE;
		//tinyxml2::XMLElement* objgrp_node = tile_node->FirstChildElement("objectgroup");
		//if (objgrp_node != nullptr) {
		//	tinyxml2::XMLElement* obj_node = objgrp_node->FirstChildElement("object");
		//	while (obj_node != nullptr) {
		//		std::string obj_type = obj_node->FindAttribute("type")->Value();
		//		if (obj_type == "Solid") {
		//			tile_collision = Tile::COLLISION_TYPE::SOLID;
		//		}
		//		obj_node = obj_node->NextSiblingElement("object");
		//	}
		//}

		/* Animation */
		AnimatedSprite* tile_sprite = nullptr;
		tinyxml2::XMLElement* anim_node = tile_node->FirstChildElement("animation");
		if (anim_node != nullptr) {
			std::vector<std::pair<unsigned int, unsigned int>> animation_tiles;
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
			tile_rect = TileIndexToRect(animation_tiles[0].first);
			// g++ just HAD to be picky about getting addresses of rvalues
			AnimatedSprite temp = AnimatedSprite(graphics, tileset_sprite_->GetTexture(), tile_rect.x, tile_rect.y, tile_rect.w, tile_rect.h, frame_speed, frame_count, tile_spacing_);
			tile_sprite = &temp;
		} else {
			// TODO 7-19-21: Replace with Sprite polymorphism
			// g++ just HAD to be picky about getting addresses of rvalues
			AnimatedSprite temp = AnimatedSprite(graphics, tileset_sprite_->GetTexture(), tile_rect.x, tile_rect.y, tile_rect.w, tile_rect.h);
			tile_sprite = &temp;
		}

		/* Store the tile in the TileList */
		tiles_[tile_id] = std::make_shared<Tile>(Tile(tile_id, tile_sprite, tile_collision));

		tile_node = tile_node->NextSiblingElement("tile");
	}
}

Tileset::Tileset(Sprite* tileset_sprite, int tile_width, int tile_height, int tile_margin, int tile_spacing) : Tileset::Tileset() {
	tileset_sprite_ = std::make_shared<Sprite>(*tileset_sprite);
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

unsigned int Tileset::GetTileCount() const {
	return tile_count_;
}

const Sprite* Tileset::GetTilesetSprite() const {
	return tileset_sprite_.get();
}

const Tile Tileset::GetTileFromTileIndex(unsigned int tile_index) const {
	if (tiles_.count(tile_index) == 0) {
		return Tile(tile_index);
	}
	Tile tile = *(tiles_.find(tile_index)->second);
	return tile;
}

void Tileset::Update(double delta_time) {
	TileList::iterator tile_iter = tiles_.begin();
	while (tile_iter != tiles_.end()) {
		tile_iter->second.get()->Update(delta_time);
		tile_iter++;
	}
}

int Tileset::Draw(Graphics& graphics, int pos_x, int pos_y, unsigned int tile_id) const {
	if (tiles_.count(tile_id) != 0) {
		const Tile* tile = tiles_.find(tile_id)->second.get();
		if (tile->GetSprite() != nullptr) {
			return tile->Draw(graphics, pos_x, pos_y);
		}
	}
	SDL_Rect tile_rect = TileIndexToRect(tile_id);
	return tileset_sprite_->Draw(graphics, pos_x, pos_y, tile_rect);
}


SDL_Rect Tileset::TileIndexToRect(unsigned int tile_index) const {
	int row_index = tile_index % tile_row_size_;
	int col_index = tile_index / tile_row_size_;
	int rect_x = row_index * (tile_width_ + tile_spacing_) + tile_margin_;
	int rect_y = col_index * (tile_height_ + tile_spacing_) + tile_margin_;
	SDL_Rect tile_rect = {
		rect_x, rect_y, tile_width_, tile_height_
	};
	return tile_rect;
}