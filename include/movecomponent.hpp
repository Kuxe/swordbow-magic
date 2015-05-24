#ifndef MOVECOMPONENT_H
#define MOVECOMPONENT_H
#include <glm/glm.hpp>

struct MoveComponent {
    glm::vec2 pos;
    glm::vec2 oldPos;
    glm::vec2 vel;
    glm::vec2 dir {0, 1};
    float maxVelLength;

	MoveComponent(
		const glm::vec2& = {0, 0},
		const glm::vec2& = {0, 0},
		const float maxVelLength = 100.0f);
};

#endif //MOVECOMPONENT_H
