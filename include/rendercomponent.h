#ifndef RENDERCOMPONENT_H
#define RENDERCOMPONENT_H

#include <string>

using namespace std;

//Class containing render-relevant data (such as image identifier)
class RenderComponent {
 public:
 	string imagePath = "UNSET";
 	float xoffset = 0;
 	float yoffset = 0;
 	float zindex = 0;

 	//If false, this entity won't re-render
 	//Note that it might still be visible if nothing was rendered above it
 	bool doRender = true;

 	//If true, will only render on doRender = true, then set doRender to false
 	bool renderOnce = false;
};

#endif //RENDERCOMPONENT_H