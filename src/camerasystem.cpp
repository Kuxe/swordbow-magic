#include "camerasystem.hpp"
#include "componentmanager.hpp"
#include "irenderer.hpp"

#include <glm/gtc/matrix_access.hpp> //glm::column
#include <glm/gtc/matrix_transform.hpp> //glm::translate

CameraSystem::CameraSystem(IRenderer* const renderer) :
    renderer(renderer) {

}

void CameraSystem::add(ID id) {
    this->cameraSource = id;
}

void CameraSystem::remove(ID id) {
    if(this->cameraSource == id) this->cameraSource = 0;
}

void CameraSystem::update() {
    //If there is a camera source (id which controlls camera)
    if(cameraSource != 0) {
        //then follow that id
        //This corresponds to updating the view matrix such that it follows
        //whatever entity pointed to by cameraSource
        const auto& mc = componentManager->moveComponents.at(cameraSource);

        //1. I need to provide side, up, forward and eye.
        //2. Each entity look in some direction. This is available of transform in movecomponent.
        //3. I could extract forward, up, side and eye from transform in movecomponent and then translate with posoff
        /**4. No I should not translate with constant vector posoff. I should translate in terms of side, up, forward.
                otherwise it the translation wont work with rotations **/
        view = mc.transform;
    }
}

unsigned int CameraSystem::count() const {
    return cameraSource > 0;
}

const System::Identifier CameraSystem::getIdentifier() const {
    return System::CAMERA;
}

bool CameraSystem::activateId(ID id) {

}

const glm::mat4& CameraSystem::getView() const {
    return view;
}

