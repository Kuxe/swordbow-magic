#include "camerasystem.hpp"
#include "componentmanager.hpp"
#include "renderer.hpp"

CameraSystem::CameraSystem(Renderer* renderer) :
    renderer(renderer),
    camera{0, 0, 640, 480} {

}

void CameraSystem::add(ID id) {
    this->cameraSource = id;
}
void CameraSystem::remove(ID id) {
    if(this->cameraSource == id) this->cameraSource = 0;
}
void CameraSystem::update() {
    if(cameraSource != 0) {
        const auto& cameraXpos = componentManager->moveComponents.at(cameraSource).pos.x - renderer->getScreenWidth()/2;
    	const auto& cameraYpos = componentManager->moveComponents.at(cameraSource).pos.y - renderer->getScreenHeight()/2;
    	camera = {
    		cameraXpos < 0 ? 0 : (int)cameraXpos,
    		cameraYpos < 0 ? 0 : (int)cameraYpos,
            renderer->getScreenWidth(),
            renderer->getScreenHeight()
    	};
    }
}
unsigned int CameraSystem::count() const {
    return cameraSource > 0;
}
const System::Identifier CameraSystem::getIdentifier() const {
    return System::CAMERA;
}
void CameraSystem::activateId(ID id) {

}

SDL_Rect& CameraSystem::getCamera() {
    return camera;
}
