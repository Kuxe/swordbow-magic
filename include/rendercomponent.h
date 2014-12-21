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
};

#endif //RENDERCOMPONENT_H