#ifndef RENDERCOMPONENT_H
#define RENDERCOMPONENT_H

#include <string>

using namespace std;

//This include is really ugly. Need it because HashGrid
//depends on width of textures. HashGrid uses rendercomponent
//to acquire these textures. So rendercomponent must have a copy
//of texturedata. Definition of texturedata is in rendersystem.
#include "rendersystem.hpp"

//Class containing render-relevant data (such as image identifier)
class RenderComponent {
 public:
 	string imagePath = "UNSET";
 	float xoffset = 0;
 	float yoffset = 0;
	TextureData textureData;

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
