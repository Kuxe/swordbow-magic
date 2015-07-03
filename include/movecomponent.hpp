#ifndef MOVECOMPONENT_H
#define MOVECOMPONENT_H
#include <glm/glm.hpp>

struct MoveComponent {
    glm::vec2 pos;
    glm::vec2 oldPos;
    glm::vec2 vel;

    //Default dir to something that could be considered undefined
    //this is neccesary because in AnimationSystem different animations
    //are selected if direction is set to anything but 0,0. This used to
    //cause segfault for bloodsplatter because bloodsplatter doesnt have
    //any "north" animation
    glm::vec2 dir {0, 0};
    float maxVelLength;

	MoveComponent(
		const glm::vec2& = {0, 0},
		const glm::vec2& = {0, 0},
		const float maxVelLength = 100.0f);
};

#endif //MOVECOMPONENT_H
