#include "entity.h"

Entity::Entity() :
	sprite_(nullptr),
	pos_x_(0.0),
	pos_y_(0.0),
	vel_x_(0.0),
	vel_y_(0.0),
	collider_({ 0, 0, 0, 0 }) {
}

Entity::~Entity() { }

std::vector<double> Entity::GetPosition() const {
	std::vector<double> position = {pos_x_, pos_y_};
	return position;
}

std::vector<double> Entity::GetVelocity() const {
	std::vector<double> velocity = { vel_x_, vel_y_ };
	return velocity;
}

void Entity::Update(const Input& input, double delta_time, const Tilemap& tilemap) { }
int Entity::Draw(Graphics& graphics, double offset_x, double offset_y) { return 0; }