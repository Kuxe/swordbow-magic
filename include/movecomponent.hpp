#ifndef MOVECOMPONENT_H
#define MOVECOMPONENT_H
#include <glm/glm.hpp>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include "cereal/archives/json.hpp"

#include "renderdata.hpp" /** Make glm::vec3 serializable by cereal **/

struct MoveComponent {
    glm::mat4 transform;
    glm::mat4 oldTransform;
    glm::vec3 velocity;

    float maxVelLength;

    MoveComponent(
    	const glm::mat4& transform = glm::mat4(),
    	const float maxVelLength = {100.0f}) :
    	transform(transform),
        oldTransform(transform),
        velocity(0.0f, 0.0f, 0.0f),
    	maxVelLength(maxVelLength) { }

    template<class Archive>
    void serialize(Archive& ar) {
        ar(transform, velocity, maxVelLength);
    }
};

#endif //MOVECOMPONENT_H
