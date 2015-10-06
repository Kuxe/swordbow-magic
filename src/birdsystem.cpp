#include "birdsystem.hpp"
#include "componentmanager.hpp"

void BirdSystem::add(ID id) {
	ids.insert(id);
}

void BirdSystem::remove(ID id) {
	ids.erase(id);
}

void BirdSystem::update() {

	//TODO: Implement.

	//Note that this approach doesn't allow for several birdswarms. But that's OK for now.

	//1.1. Randomize an swarmAcceleration-vector with higher probability pointing towards fixedPoint

	//1.2. Normalize swarmAcceleration to a constant length

	//1.3. Update acceleration of swarmPoint

	for(auto id : ids) {

		//2.1. Randomize an birdAcceleration-vector with higher probability pointing towards swarmPoint 
		glm::vec2 accelerationVector = {0, 0};

		//2.2. Normalize birdAcceleration to a constant length
		constexpr float accelerationNorm = 0.5f;

		//2.3. Update acceleration of bird
		auto& accelerationComponent = componentManager->accelerationComponents.at(id);
		accelerationComponent.vec += accelerationVector;
	}
}

unsigned int BirdSystem::count() const {
	return ids.size();
}

const System::Identifier BirdSystem::getIdentifier() const {
	return System::BIRD;
}

bool BirdSystem::activateId(ID id) {
	return false;
}

void BirdSystem::setFixedPoint(const glm::vec2& fixedPoint) {
	this->fixedPoint = fixedPoint;
}