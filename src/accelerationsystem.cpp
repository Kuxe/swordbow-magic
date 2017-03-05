#include "accelerationsystem.hpp"
#include "componentmanager.hpp"
#include "logger.hpp"

void AccelerationSystem::add(ID id) {
	ids.insert(id);
}

void AccelerationSystem::remove(ID id) {
	ids.erase(id);
}

void AccelerationSystem::update() {
	for(auto id : ids) {
		auto& mc = componentManager->moveComponents.at(id);
		const auto& ac = componentManager->accelerationComponents.at(id);
		mc.velocity += ac.acceleration * deltaTime->delta();
	}
}

unsigned int AccelerationSystem::count() const {
	return ids.size();
}

const System::Identifier AccelerationSystem::getIdentifier() const {
	return System::ACCELERATION;
}

bool AccelerationSystem::activateId(ID id) {
	return false;
}