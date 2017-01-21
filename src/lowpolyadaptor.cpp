#include "lowpolyadaptor.hpp"

/** For logging **/
#include "logger.hpp"
#include <ostream>

#include "iclientstate.hpp"

LowpolyAdaptor::LowpolyAdaptor(bool fullscreen, bool vsync) {
	renderer.initialize(this);
}

LowpolyAdaptor::~LowpolyAdaptor() {
	void terminate();
}

void LowpolyAdaptor::run() {
	SphereGenerator sphereGenerator({125.0f, 125.0f, 125.0f}, 3);
	TerrainGenerator terrainGenerator;
	renderer.loadModel("sphere", sphereGenerator.generate());
	renderer.loadModel("terrain", terrainGenerator.generate());
	if(!renderer.render(*this)) {
		Logger::error("Lowpoly3d renderer failed");
	}
}

void LowpolyAdaptor::render() {
	signalRenderer();
}

void LowpolyAdaptor::renderOnlyOverlays() {

}

void LowpolyAdaptor::renderTexts() {

}

void LowpolyAdaptor::showOverlay(const Image::Identifier& identifier, const Text& text) {

}

void LowpolyAdaptor::hideOverlay(const Image::Identifier& identifier) {

}

void LowpolyAdaptor::fadeInOverlay(const Image::Identifier& identifier, float seconds, const Text& text) {

}

void LowpolyAdaptor::fadeOutOverlay(const Image::Identifier& identifier, float seconds) {

}

void LowpolyAdaptor::renderOverlays() {
    
}

void LowpolyAdaptor::printText(const Text& text) {

}

void LowpolyAdaptor::pollEvents(IClientState* clientState) {
	//TODO: Implement
	/** Should poll events such as what input happened, if window was resized etc
		Lowpoly3d should probably store all event it obtains in an eventqueue which
		is accesible from outside lowpoly3d **/

	//Loop through each event in eventqueue
	//Give it to clientState. clientState decides what to do with it.

	//clientDisconnectState should set client.running=false on a quit-event
	//clientReceiveInitialState should set client.running=false on a quit-event
	//clientRunningState should set client.running=false on a quit-event and push_back keypress into presses if key is pressed and push_back keyrelease into releases if key is released
}

const std::vector<RenderData>& LowpolyAdaptor::getRenderDatas() const {
	return rds;
}

const glm::mat4 LowpolyAdaptor::getView() const {
	return glm::mat4();
}

const float LowpolyAdaptor::getGametime() const {
	return 0.0f;
}

const float LowpolyAdaptor::getSunRadians() const {
	return 0.0f;
}

void LowpolyAdaptor::onKey(int key, int scancode, int action, int mods) {

}

void LowpolyAdaptor::onMouse(double xpos, double ypos) {

}
