#include "movecomponent.hpp"

MoveComponent::MoveComponent(
	const Point& pos,
	const Point& oldPos,
	const Vector2& vel)
	:
	pos(pos),
	oldPos(pos),
	vel(vel) { }

MoveComponent::~MoveComponent() {

}

void MoveComponent::move() {
	pos.x += vel.x;
	pos.y += vel.y;
}
