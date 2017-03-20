#include "deltatime.hpp"
#include "systemmanager.hpp"
#include "movesystem.hpp"
#include "componentmanager.hpp"
#include "movecomponent.hpp"
#include "inputcomponent.hpp"
#include "icommand.hpp"

#include <glm/gtc/matrix_transform.hpp> //glm::translate

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

		//If this entity has an inputComponent,
		//check if buttons are pressed in order to move entity.
		//This should probably be a system of its own that's being
		//run before movesystem (requiring movecomponents and inputcomponents,
		//a system for handling input->velocity on entities. Ideally, this 
		//system should _only_ be engaged in updating positions...)
		if( componentManager->inputComponents.find(id) !=
			componentManager->inputComponents.end()) {
			auto& ic = componentManager->inputComponents.at(id);
		}

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
		/*if(!(mc.pos.x == mc.oldPos.x && mc.pos.y == mc.oldPos.y)) {

			//But only if it has a commandComponent. This "onMove" events
			//should probably be handled in a system of its own, ie a
			//"onMoveSystem".. Maybe. This will be sufficient for now.
			if(componentManager->commandComponents.find(id) != componentManager->commandComponents.end()) {
				auto& cc = componentManager->commandComponents.at(id);
				cc.execute(CommandComponent::Event::ON_MOVE);
			}
		}*/
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
