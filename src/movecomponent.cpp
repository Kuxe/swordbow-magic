#include "movecomponent.hpp"

MoveComponent::MoveComponent(
	const float& xpos,
	const float& ypos,
	const float& oldXpos,
	const float& oldYpos,
	const float& xspeed,
	const float& yspeed)
	:
	xpos(xpos),
	ypos(ypos),
	oldXpos(oldXpos),
	oldYpos(oldYpos),
	xspeed(xspeed),
	yspeed(yspeed) {

}

MoveComponent::~MoveComponent() {

}

void MoveComponent::move() {
	xpos += xspeed;
	ypos += yspeed;
}
