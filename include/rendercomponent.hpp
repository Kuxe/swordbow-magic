#ifndef RENDERCOMPONENT_H
#define RENDERCOMPONENT_H

#include <string>

//Class containing render-relevant data (such as image identifier)
class RenderComponent {
 public:
 	string imagePath = "UNSET";
 	float xoffset = 0;
 	float yoffset = 0;

	//is set to y+height during runtime
	//used in painters algorithm
 	float zindex = 0;

	//Its not obvious how zindex_base differs from zindex:
	//zindex_base doesnt update to y+height, but is a
	//(typically) constant value. Used to force something
	//to be rendered before or after whatever is governed by zindex
	float zindex_base = 0;
};

#endif //RENDERCOMPONENT_H
