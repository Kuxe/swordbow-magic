#include "camerasystem.hpp"
#include "componentmanager.hpp"

CameraSystem::CameraSystem(RenderSystem* renderSystem) :
    renderSystem(renderSystem),
    camera{0, 0, 640, 480} {

}

void CameraSystem::add(ID id) {
    this->cameraSource = id;
}
void CameraSystem::remove(ID id) {
    if(this->cameraSource == id) this->cameraSource = 0;
}
void CameraSystem::update() {
    const auto& cameraXpos = componentManager->moveComponents.at(cameraSource).xpos - renderSystem->getScreenWidth()/2;
	const auto& cameraYpos = componentManager->moveComponents.at(cameraSource).ypos - renderSystem->getScreenHeight()/2;
	camera = {
		cameraXpos < 0 ? 0 : (int)cameraXpos,
		cameraYpos < 0 ? 0 : (int)cameraYpos,
        renderSystem->getScreenWidth(),
        renderSystem->getScreenHeight()
	};
}
unsigned int CameraSystem::count() const {
    return cameraSource > 0;
}
const string CameraSystem::getIdentifier() const {
    return "CameraSystem";
}
void CameraSystem::activateId(ID id) {

}

SDL_Rect& CameraSystem::getCamera() {
    return camera;
}
