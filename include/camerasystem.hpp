#ifndef CAMERASYSTEM_HPP
#define CAMERASYSTEM_HPP

#include "isystem.hpp"
#include <glm/mat4x4.hpp>

/** This system produces a view matrix which is used by IRenderer.
	CameraSystem needs a cameraSource which is set upon calling add(ID id).
	So this system is different from other systems because it only has one
	id at any time, the id which the camera should follow. **/

typedef unsigned int ID;
class IRenderer;
class CameraSystem : public ISystem {
private:
    ID cameraSource = 0;
    IRenderer* const renderer;
    glm::mat4 view;
public:
    CameraSystem(IRenderer* const renderer);
    void add(ID id);
    void remove(ID id);
    void update();
    unsigned int count() const;
    const System::Identifier getIdentifier() const;
    bool activateId(ID id);
    const glm::mat4& getView() const;
};


#endif //CAMERASYSTEM_HPP
