#include "entity.h"

Entity::Entity() {
	sprite_ = nullptr;
	pos_x_ = 0.0;
	pos_y_ = 0.0;
	vel_x_ = 0.0;
	vel_y_ = 0.0;
	collider_ = { 0, 0, -1, -1 };
}

Entity::~Entity() { }

void Entity::Update() { }
void Entity::Update(const Uint8* keyboard_state) { }
int Entity::Draw(Graphics& graphics) { return 0; }