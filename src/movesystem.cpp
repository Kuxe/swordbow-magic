#include "deltatime.hpp"
#include "systemmanager.hpp"
#include "movesystem.hpp"
#include "componentmanager.hpp"
#include "movecomponent.hpp"
#include "inputcomponent.hpp"
#include "icommand.hpp"

#include <glm/gtc/matrix_transform.hpp> //glm::translate
#include <glm/gtc/matrix_access.hpp> //glm::column

/** For logging **/
#include "logger.hpp"

void MoveSystem::add(ID id) {
	ids.insert(id);
	activeIds.insert(id);
}

void MoveSystem::remove(ID id) {
	if(ids.erase(id) == 0) {
		Logger::log("MoveSystem tried to erase unpresent ID" + std::to_string(id), Logger::WARNING);
		return;
	}
}

void MoveSystem::update() {
	for(auto id : ids) {
		auto& mc = componentManager->moveComponents.at(id);

		glm::vec3 direction = {0.0f, 0.0f, 0.0f};

		//If this entity has an inputComponent,
		//check if buttons are pressed in order to move entity.
		//This should probably be a system of its own that's being
		//run before movesystem (requiring movecomponents and inputcomponents,
		//a system for handling input->velocity on entities. Ideally, this 
		//system should _only_ be engaged in updating positions...)
		mc.oldTransform = mc.transform;
		if( componentManager->inputComponents.find(id) !=
			componentManager->inputComponents.end()) {
			auto& ic = componentManager->inputComponents.at(id);

			//Set velocity to work in the direction of WASD movement
			//holding two opposite keys ie A and D will cancel out
			direction = {ic.a - ic.d, ic.q - ic.e, ic.w - ic.s};

			//If entity has moved its mouse, rotate the view transform matrix
			const glm::vec2 MOUSE_SENSITIVITY(0.01f, 0.003f);
			const float horizontalMouseMovement = MOUSE_SENSITIVITY.x*(ic.mousePos.x - ic.oldMousePos.x);
			const float verticalMouseMovement = MOUSE_SENSITIVITY.y*(ic.mousePos.y - ic.oldMousePos.y);
			mc.transform = glm::rotate(mc.transform, horizontalMouseMovement, glm::vec3(0.0f, 1.0f, 0.0f));
			//FIXME: Figure out to rotate camera around x-axis mc.transform = glm::rotate(mc.transform, verticalMouseMovement, glm::vec3(glm::column(mc.transform, 2)));
			//Finally update oldMousePos
			ic.oldMousePos = ic.mousePos;
		}

		/** Change direction into basis of entity so that pressing 'w' actually makes
			player go forward as opposed to going in {0.0f, 0.0f, 1.0f} direction**/
		mc.velocity = glm::inverse(glm::mat3(mc.transform)) * direction;

		//If some input was recieved which caused a move (mc.vel isn't of length 0)
		if(glm::length(mc.velocity) > 0) {
			if(glm::length(mc.velocity) > mc.maxVelLength) {
				mc.velocity = glm::normalize(mc.velocity);
				mc.velocity *= mc.maxVelLength;
			}


			//Adjust for dt
			mc.velocity *= deltaTime->delta();

			//Finally update position
			mc.transform = glm::translate(mc.transform, mc.velocity);
		}

		//Do something defined by the entity if the entity moved

		//TODO: Aha, this is why I need an "oldPos" member (I realize this as of 2017-03-06)
		//Ill comment this out and reintroduce oldPos in later stages
		if(mc.transform != mc.oldTransform) {

			//But only if it has a commandComponent. This "onMove" events
			//should probably be handled in a system of its own, ie a
			//"onMoveSystem".. Maybe. This will be sufficient for now.
			if(componentManager->commandComponents.find(id) != componentManager->commandComponents.end()) {
				auto& cc = componentManager->commandComponents.at(id);
				cc.execute(CommandComponent::Event::ON_MOVE);
			}
		}
	}
}

unsigned int MoveSystem::count() const {
	return ids.size();
}

const System::Identifier MoveSystem::getIdentifier() const {
	return System::MOVE;
}

bool MoveSystem::activateId(ID id) {
	add(id);
	return true;
}
