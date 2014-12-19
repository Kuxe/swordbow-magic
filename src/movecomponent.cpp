#include "movecomponent.h"

MoveComponent::MoveComponent(
	const float& xpos,
	const float& ypos,
	const float& xspeed,
	const float& yspeed)
	: xpos(xpos), ypos(ypos), xspeed(xspeed), yspeed(yspeed) {

}

MoveComponent::~MoveComponent() {

}

void MoveComponent::move() {
	xpos += xspeed;
	ypos += yspeed;
}