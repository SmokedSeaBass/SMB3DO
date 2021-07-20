#include "level.h"

#include "camera.h"
#include "player.h"

Level::Level() {
	display_name_ = std::string("Test Level");
	timelimit_ = 999;
	bounds_ = {0, 0, 0, 0};
	entity_list_ = std::vector<std::shared_ptr<Entity>>();
	tileset_list_ = std::vector<Tileset*>();
	tilemap_layers_ = std::vector<Tilemap>();
}

std::string Level::GetDisplayName() const {
	return display_name_;
}

void Level::SetDisplayName(std::string display_name) {
	display_name_ = display_name;
}

int Level::Load(Graphics& graphics, const std::string& path_to_lvl) {
	// Load Tilemaps and Tilesets
	Tilemap tilemap = Tilemap(graphics, path_to_lvl);
	bounds_ = { 0, 0, tilemap.GetDimensions()[0], tilemap.GetDimensions()[1] };
	for (Tileset* tileset : tilemap.GetTilesets()) {
		tileset_list_.push_back(tileset);
	}
	tilemap_layers_.push_back(tilemap);

	// Create actors
	Player* mario = new Player(graphics, "assets/sprite_sheets/mario.bmp", 32, 440);
	entity_list_.push_back(std::shared_ptr<Player>(mario));

	camera_ = Camera(graphics.GetViewport());
	camera_.SetTarget(Camera::CameraTarget(*mario));
	camera_.SetPosition(0, 12 * 16);
	camera_.SetBounds(Rectangle(bounds_.x * TILESIZE_NES, bounds_.y * TILESIZE_NES, bounds_.w * TILESIZE_NES, bounds_.h * TILESIZE_NES));
	camera_.SetCaptureBounds(Rectangle(-8, 0, 16, 0));
	camera_.SetVerticalScrollRule(Camera::VerticalScrollRule::SELECTIVE);
	camera_.SetInterpRatio(1.0f);

	return 0;
}

void Level::Update(Graphics& graphics, const Input& input, double delta_time) {
	// Update entities
	std::vector<std::shared_ptr<Entity>>::iterator entity_iter;
	for (entity_iter = entity_list_.begin(); entity_iter != entity_list_.end(); entity_iter++) {
		(*entity_iter)->Update(input, delta_time, tilemap_layers_[0]);
	}

	// Update tilemaps
	for (Tilemap tilemap : tilemap_layers_) {
		tilemap.Update(delta_time);
	}

	// Update camera
	camera_.SetDimensions(graphics.GetViewport().w, graphics.GetViewport().h);
	camera_.Update(delta_time);
}

int Level::Draw(Graphics& graphics) {
	// Draw background color
	graphics.DrawColoredRect(NULL, 0xAF, 0xE5, 0xEA, 0xFF);

	// Draw tilemaps
	for (Tilemap tilemap : tilemap_layers_) {
		tilemap.Draw(graphics, -static_cast<int>(floor(camera_.GetPosition()[0])), -static_cast<int>(floor(camera_.GetPosition()[1])));
	}

	// Draw entities
	std::vector<std::shared_ptr<Entity>>::iterator entity_iter;
	for (entity_iter = entity_list_.begin(); entity_iter != entity_list_.end(); entity_iter++) {
		(*entity_iter)->Draw(graphics, -static_cast<int>(floor(camera_.GetPosition()[0])), -static_cast<int>(floor(camera_.GetPosition()[1])));
	}


	// UI
	return 0;
}
