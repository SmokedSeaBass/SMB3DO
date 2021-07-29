#include "level.h"

#include "camera.h"
#include "game.h"
#include "player.h"

Level::Level() {
	display_name_ = std::string("Test Level");
	timelimit_ = 999;
	background_color_ = { 0, 0, 0, 0 };
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
	background_color_ = tilemap.GetBackgroundColor();
	bounds_ = { 0, 0, tilemap.GetDimensions()[0], tilemap.GetDimensions()[1] };
	for (Tileset* tileset : tilemap.GetTilesets()) {
		tileset_list_.push_back(tileset);
	}
	tilemap_layers_.push_back(tilemap);

	// Create actors
	Player* player = new Player(graphics, "assets/sprite_sheets/mario.bmp", 32, 440);
	entity_list_.push_back(std::shared_ptr<Player>(player));

	camera_ = Camera(graphics.GetViewport());
	camera_.SetTarget(Camera::CameraTarget(*player));
	camera_.SetPosition(0, 12 * 16);
	camera_.SetBounds(Rectangle(bounds_.x * (int)TILESIZE_NES, bounds_.y * (int)TILESIZE_NES, bounds_.w * (int)TILESIZE_NES, bounds_.h * (int)TILESIZE_NES));
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
	camera_.SetDimensions(graphics.GetCanvasDimensions().w, graphics.GetCanvasDimensions().h);
	camera_.Update(delta_time);
}

int Level::Draw(Graphics& graphics) {
	// Draw background color
	if (background_color_.a > 0x00) {
		graphics.DrawColoredRect(NULL, background_color_.r, background_color_.g, background_color_.b, background_color_.a);
	}

	// Draw tilemaps
	for (Tilemap tilemap : tilemap_layers_) {
		tilemap.Draw(graphics, -(int)floor(camera_.GetPosition()[0]), -(int)floor(camera_.GetPosition()[1]), {0, 0, camera_.GetDimensions()[0], camera_.GetDimensions()[1] });
	}

	// Draw entities
	std::vector<std::shared_ptr<Entity>>::iterator entity_iter;
	for (entity_iter = entity_list_.begin(); entity_iter != entity_list_.end(); entity_iter++) {
		(*entity_iter)->Draw(graphics, -static_cast<int>(floor(camera_.GetPosition()[0])), -static_cast<int>(floor(camera_.GetPosition()[1])));
	}

	// UI
	if (Game::debug_show_info) {
		Player* player = dynamic_cast<Player*>(entity_list_[0].get());
		graphics.SetTextFont("dialogue");
		graphics.DrawText("X: " + std::to_string(player->GetPosition()[0]), 0, 16);
		graphics.DrawText("Y: " + std::to_string(player->GetPosition()[1]), 0, 24);
		graphics.DrawText("VX: " + std::to_string(player->GetVelocity()[0]), 0, 32);
		graphics.DrawText("VY: " + std::to_string(player->GetVelocity()[1]), 0, 40);
		graphics.DrawText("CX: " + std::to_string(camera_.GetPosition()[0]), 0, 48);
		graphics.DrawText("CY: " + std::to_string(camera_.GetPosition()[1]), 0, 56);
		graphics.SetTextFont("hud");
		graphics.DrawText("$" + std::to_string(player->coin_count_), 0, 64);
		graphics.SetTextFont("dialogue");
		graphics.DrawText("\"DIALOGUE\" can be <UPPERCASE'd>\n and {lowercase'd}!?", 0, 80);
		graphics.SetTextFont("hud");
		graphics.DrawText("Hide me with [U]", 0, 104);
	}

	return 0;
}
