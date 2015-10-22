#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include "rect.hpp"
#include "systemidentifiers.hpp"
#include <glm/vec2.hpp>

class ComponentManager;

typedef unsigned int ID;
class BoundingBox {
protected:
        const ComponentManager* const componentManager;
public:

    BoundingBox(const ComponentManager* const componentManager) :
        componentManager(componentManager) {}
    virtual const Rect getBoundingBox(ID id) const = 0;
    virtual const Rect getOldBoundingBox(ID id) const = 0;
    virtual const System::Identifier getSystemName() const = 0;
};

static inline constexpr Rect getBoundingBox(
	const glm::vec2& position,
	const glm::vec2& dimension,
	const glm::vec2& offset = {0.0f, 0.0f}) {

    return Rect {
	    floorf(position.x + offset.x), //TODO: just add the vectors and floor resulting vector
	    floorf(position.y + offset.y),
	    static_cast<float>(dimension.x),
	    static_cast<float>(dimension.y)
	};
}





#endif //BOUNDINGBOX_H
