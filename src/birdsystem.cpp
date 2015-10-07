#include "birdsystem.hpp"
#include "componentmanager.hpp"
#include <glm/gtx/vector_angle.hpp>
#include <chrono>

BirdSystem::BirdSystem() {
	seedTimer.start();
}

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
		auto& birdComponent = componentManager->birdComponents.at(id);
		auto& moveComponent = componentManager->moveComponents.at(id);
		auto& accelerationComponent = componentManager->accelerationComponents.at(id);
		auto& swarmIndex = birdComponent.swarmIndex;
		auto& swarmPoint = swarmPoints.at(swarmIndex);
		accelerationComponent.vec += getRandomAcceleration(moveComponent.pos, swarmPoint, 1600);
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

unsigned char BirdSystem::createSwarm(const glm::vec2& fixedPoint) {
	auto swarmId = idManager.acquireId();
	fixedPoints.insert({swarmId, fixedPoint});
	//TODO: Generate swarmPoint
	glm::vec2 swarmPoint = {50, 50};
	swarmPoints.insert({swarmId, swarmPoint});
	return swarmId;
}

bool BirdSystem::dissolveSwarm(SwarmIndex swarmIndex) {
	if( fixedPoints.find(swarmIndex) != fixedPoints.end() &&
		swarmPoints.find(swarmIndex) != swarmPoints.end()) {

		fixedPoints.erase(swarmIndex);
		swarmPoints.erase(swarmIndex);
		return true;
	}
	return false;
}

glm::vec2 BirdSystem::getRandomAcceleration(
	const glm::vec2& v1,
	const glm::vec2& v2,
	const unsigned short variance) const {
	
	unsigned short angleBetweenSwarmAndBird = glm::angle(v1, v2);
	std::normal_distribution<double> normalDistribution = std::normal_distribution<double>(angleBetweenSwarmAndBird, variance);
	std::default_random_engine generator(seedTimer.elapsed()*1000000000.0f);
	auto randomAngle = normalDistribution(generator);
	return glm::vec2 {cos(randomAngle), sin(randomAngle)} * glm::length(v1 - v2);
}