#ifndef RENDERCOMPONENT_H
#define RENDERCOMPONENT_H

#include "imageidentifiers.hpp"
#include <glm/vec2.hpp>

//Class containing render-relevant data (such as image identifier)
class RenderComponent {
 public:
 	Image::Identifier image = Image::UNDEFINED;
 	glm::vec2 offset = {0.0f, 0.0f};

	//is set to y+height during runtime
	//used in painters algorithm
 	float zindex = 0;

	//Its not obvious how zindex_base differs from zindex:
	//zindex_base doesnt update to y+height, but is a
	//(typically) constant value. Used to force something
	//to be rendered before or after whatever is governed by zindex
	float zindex_base = 0;

    template<class Archive>
    void serialize(Archive& ar) {
        ar(image, offset.x, offset.y, zindex, zindex_base);
    }
};

#endif //RENDERCOMPONENT_H
