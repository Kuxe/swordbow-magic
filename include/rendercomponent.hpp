#ifndef RENDERCOMPONENT_H
#define RENDERCOMPONENT_H

#include <string>

//Class containing render-relevant data (such as image identifier)
struct RenderComponent {
 	std::string model = "sphere", shader = "default";
    template<class Archive>
    void serialize(Archive& ar) {
        ar(model, shader);
    }
};

#endif //RENDERCOMPONENT_H
