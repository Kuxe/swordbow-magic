#include "deltatime.hpp"
#include "systemmanager.hpp"
#include "movesystem.hpp"
#include "componentmanager.hpp"
#include "movecomponent.hpp"
#include "inputcomponent.hpp"
#include "icommand.hpp"

/** For logging **/
#include "logger.hpp"

void MoveSystem::add(ID id) {
	ids.insert(id);
	activeIds.insert(id);
	systemManager->getSystem(System::MOVEDIFF)->add(id);
}

void MoveSystem::remove(ID id) {
	if(ids.erase(id) == 0) {
		Logger::log("MoveSystem tried to erase unpresent ID" + std::to_string(id), Log::WARNING);
		return;
	}

	//since ID isnt moving anymore, oldxpos should be equal to xpos
	//but that update wont happen within update() method since
	//this id wont be iterated over within that loop after this call
	//so it is neccesary to force-update oldxpos here
	auto& mc = componentManager->moveComponents.at(id);
	mc.oldPos.x = mc.pos.x;
	mc.oldPos.y = mc.pos.y;
}

void MoveSystem::update() {
	for(auto id : ids) {
		auto& mc = componentManager->moveComponents.at(id);
		auto& ic = componentManager->inputComponents.at(id);
		auto& cc = componentManager->commandComponents.at(id);
		mc.oldPos.x = mc.pos.x;
		mc.oldPos.y = mc.pos.y;

		//If no key was pressed
		if(ic.d != 0 || ic.a != 0 || ic.s != 0 || ic.w != 0) {
			mc.dir.x = ic.d - ic.a;
			mc.dir.y = ic.s - ic.w;
		}

		mc.vel = {ic.d - ic.a, ic.s - ic.w};

		//If some input was recieved which caused a move (mc.vel isn't of length 0)
		if(glm::length(mc.vel) > 0) {
			//Set it to appropiate length
			mc.vel = glm::normalize(mc.vel);
			mc.vel *= mc.maxVelLength;

			//Adjust for dt
			mc.vel *= deltaTime->delta();

			//Finally update position
			mc.pos += mc.vel;

			//It might be handy to normalize the dir for other systems
			mc.dir = normalize(mc.dir);
		}

		//Do something defined by the entity if the entity moved
		if(!(mc.pos.x == mc.oldPos.x && mc.pos.y == mc.oldPos.y)) {
			cc.execute(CommandComponent::Event::ON_MOVE);

			systemManager->getSystem(System::MOVEDIFF)->add(id);
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
