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

        //Use a simple "follow" camera
        const glm::vec3 center = glm::vec3(glm::column(mc.transform, 3));
        //Direciton is negated since we're looking in negative Z
        const glm::vec3 direction = -glm::vec3(glm::column(mc.transform, 2));
        const glm::vec3 up = {0.0f, 1.0f, 0.0f};
        const float upMultipler = 0.4f;
        const float distanceToEntityMultiplier = 10.0f;

        const glm::vec3 eye = center - ((direction-up*upMultipler)*distanceToEntityMultiplier);
        view = glm::lookAt(eye, center, up);
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

