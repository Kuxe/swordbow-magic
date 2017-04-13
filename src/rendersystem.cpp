#include "rendersystem.hpp"
#include "deltatime.hpp"
#include "componentmanager.hpp"
#include "rendercomponent.hpp"
#include "movecomponent.hpp"
#include "spatialindexer.hpp"
#include "camerasystem.hpp"
#include "irenderer.hpp"
#include "logger.hpp"
#include "timer.hpp"
#include "boundingbox.hpp"

RenderSystem::RenderSystem(IRenderer* const renderer, SpatialIndexer* spatialIndexer, CameraSystem* cameraSystem) :
    renderer(renderer), spatialIndexer(spatialIndexer), cameraSystem(cameraSystem) { }

void RenderSystem::add(ID id) {
	ids.insert(id);
	activeIds.push_back(id);
}

void RenderSystem::remove(ID id) {
	ids.erase(id);
}

void RenderSystem::update() {
    //This place would be a nice place to perform various culling (or not, various culling should be a feature of Lowpoly3d)
    //Active ids in this system should be sent to lowpoly3d

	//Tell IRenderer that the scene should be rendered
    renderer->render(activeIds, *componentManager, cameraSystem->getView());
}

unsigned int RenderSystem::count() const {
	return ids.size();
}

const System::Identifier RenderSystem::getIdentifier() const {
	return System::RENDER;
}

bool RenderSystem::activateId(ID id) {
	//Only make the id active if it is a member of rendersystem
	if(ids.find(id) != ids.end()) {
		activeIds.push_back(id);
        return true;
	}
    return false;
}

void RenderSystem::printText(const Text& text) {
    //renderer->printText(text);
}
