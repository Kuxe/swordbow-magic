#include "movecomponent.hpp"

MoveComponent::MoveComponent(
	const Point& pos,
	const Point& oldPos,
	const Vector2& vel)
	:
	pos(pos),
	oldPos(pos),
	vel(vel) { }
