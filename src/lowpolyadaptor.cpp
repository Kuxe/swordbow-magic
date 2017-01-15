#include "lowpolyadaptor.hpp"

/** For logging **/
#include "logger.hpp"
#include <ostream>

#include "iclientstate.hpp"

LowpolyAdaptor::LowpolyAdaptor(bool fullscreen, bool vsync) {

}

LowpolyAdaptor::~LowpolyAdaptor() {

}

void LowpolyAdaptor::render() {

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
