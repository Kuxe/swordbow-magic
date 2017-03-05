#ifndef MOVECOMPONENT_H
#define MOVECOMPONENT_H
#include <glm/glm.hpp>

struct MoveComponent {
    glm::mat4 transform;
    glm::vec3 velocity;

    float maxVelLength;

    MoveComponent(
    	const glm::mat4& transform = glm::mat4(),
    	const float maxVelLength = {100.0f}) :
    	transform(transform),
    	maxVelLength(maxVelLength) { }

    template<class Archive>
    void serialize(Archive& ar) {
        //TODO: Convert from 2D to 3D ar(transform, velocity, maxVelLength);
    }
};

#endif //MOVECOMPONENT_H
