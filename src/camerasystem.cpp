#include "camerasystem.hpp"
#include "componentmanager.hpp"
#include "irenderer.hpp"

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
        //TODO: Implement
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

