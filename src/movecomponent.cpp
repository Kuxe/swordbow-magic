#include "movecomponent.hpp"

MoveComponent::MoveComponent(
	const glm::vec2& pos,
	const glm::vec2& oldPos,
	const float maxVelLength)
	:
	pos(pos),
	oldPos(pos),
	maxVelLength(maxVelLength) { }
