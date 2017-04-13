#include "lowpolyadaptor.hpp"

/** For logging **/
#include "logger.hpp"
#include <ostream>

/** For converting active ids to lowpoly3d format **/
#include "componentmanager.hpp"

#include "iclientstate.hpp"

#include "deltatime.hpp" //lowpoly3d::Camera need it to run smooth

using namespace lowpoly3d;

LowpolyAdaptor::LowpolyAdaptor(bool fullscreen, bool vsync) {
	const std::string shaderDirectory = "shaders/";
	renderer.initialize(this, shaderDirectory);
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

void LowpolyAdaptor::render(const std::vector<ID>& activeIds, const ComponentManager& cm, const glm::mat4& view) {
	/** Lowpoly3d reads from member "rds" so here I should convert
		activeIds (the set of all entities which should be rendered)
		and populate "rds" with the conversions. This is why componentmanager is known
		here, so that actual data can be converted into something which lowpoly3d digest...
		only ids can not be interpreted by lowpoly3d! **/

	//Currently I wont bother thinking about doing clever conversions (= no copying of data)

	rds.clear();
	for(const ID& id : activeIds) {
		const auto& rd = cm.renderComponents.at(id);
		const auto& mc = cm.moveComponents.at(id);
		rds.push_back({mc.transform, rd.model, rd.shader});
	}

	this->view = view;

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
	/** Should poll events such as what input happened, if window was resized etc
		All events that lowpoly3d may produce such as key-events or quit events etc
		*should* be available via the ILowpolyInput interface. It up to the user (swordbow-magic)
		to decide what to do with these events.
		So swordbow-magic decides wether to use event queue or not. Lowpoly merely forwards events.
		Enough rambling, swordbow-magic should react on "onKey" and "onMouse" methods by putting
		typed events in a queue. These types are provided by swordbow-magic, not lowpoly3d.
		So swordbow-magic will always know how to process these events no matter the underlying
		rendering framework. Lowpoly adaptor is responsible for converting events from lowpoly
		to events understood by swordbow magic. These events (POD structs) are stored in "events.hpp" **/

	//Loop through each event in eventqueue
	//Give it to clientState. clientState decides what to do with it.
	for(const KeyEvent& evt : keyEvents) {
		clientState->onEvent(evt);
	}

	for(const MouseEvent& evt : mouseEvents) {
		clientState->onEvent(evt);
	}

	keyEvents.clear();
	mouseEvents.clear();
}

const std::vector<RenderData>& LowpolyAdaptor::getRenderDatas() const {
	return rds;
}

const glm::mat4 LowpolyAdaptor::getView() const {
	return view;
}

using namespace std::chrono;
const float LowpolyAdaptor::getGametime() const {
	/** TODO: Use servertime. Need to send servertime at some interval from server
		and make sure that client uses that time. Dont want to send servertime to often
		since client and server probably wont desync clocks to often. **/
	static decltype(high_resolution_clock::now()) startTime = high_resolution_clock::now();
	using ms = duration<float, std::milli>;
	return duration_cast<ms>(high_resolution_clock::now() - startTime).count() / 1000.0f;
}

const float LowpolyAdaptor::getSunRadians() const {
	return .01f*getGametime();
}

void LowpolyAdaptor::onKey(int key, int scancode, int action, int mods) {
	if(action == GLFW_PRESS) {
		keyEvents.push_back({key, true});
	}
	if(action == GLFW_RELEASE) {
		keyEvents.push_back({key, false});
	}
}

void LowpolyAdaptor::onMouse(double xpos, double ypos) {
	mouseEvents.push_back({static_cast<float>(xpos), static_cast<float>(ypos)});
}
