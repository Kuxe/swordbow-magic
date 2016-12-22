#include "birdsystem.hpp"
#include "componentmanager.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/vector_angle.hpp>
#include <chrono>
#include "deltatime.hpp"
#include <math.h>
#include "entitymanager.hpp"

BirdSystem::BirdSystem(EntityManager* const entityManager) :
	entityManager(entityManager) {
	seedTimer.start();
}

void BirdSystem::add(ID id) {
	birdIds.insert(id);
}

void BirdSystem::remove(ID id) {
	birdIds.erase(id);
}

void BirdSystem::update() {
	for(const auto& id : swarmIds) {
		//TODO: fixedPoint could be data in a swarmcomponent, right?§
		const auto& fixedPoint = fixedPoints.at(id);
		const auto& moveComponent = componentManager->moveComponents.at(id);
		auto& accelerationComponent = componentManager->accelerationComponents.at(id);
		accelerationComponent.vec = accelerationComponent.vec*0.9995f + getRandomAcceleration(moveComponent.pos - fixedPoint, 2000, 0.001f);
	}

	for(auto id : birdIds) {
		const auto& birdComponent = componentManager->birdComponents.at(id);
		const auto& moveComponent = componentManager->moveComponents.at(id);
		const auto& swarmId = birdComponent.swarmId;
		const auto& swarmPoint = componentManager->moveComponents.at(swarmId).pos;
		auto& accelerationComponent = componentManager->accelerationComponents.at(id);
		accelerationComponent.vec = accelerationComponent.vec*0.995f + getRandomAcceleration(moveComponent.pos - swarmPoint, 333, 0.01f);
	}
}

unsigned int BirdSystem::count() const {
	return birdIds.size();
}

const System::Identifier BirdSystem::getIdentifier() const {
	return System::BIRD;
}

bool BirdSystem::activateId(ID id) {
	return false;
}

ID BirdSystem::createSwarm(const glm::vec2& fixedPoint) {
	//TODO: Generate swarmPoint
	glm::vec2 position = {0, 400};
	auto swarmId = entityManager->createSwarmPoint(position);
	fixedPoints.insert({swarmId, fixedPoint});
	swarmIds.insert(swarmId);
	return swarmId;
}

bool BirdSystem::dissolveSwarm(ID swarmId) {
	if( fixedPoints.find(swarmId) != fixedPoints.end() &&
		swarmIds.find(swarmId) != swarmIds.end()) {

		fixedPoints.erase(swarmId);
		swarmIds.erase(swarmId);
		entityManager->remove(swarmId);
		return true;
	}
	return false;
}

glm::vec2 BirdSystem::getRandomAcceleration(
	const glm::vec2& v,
	const unsigned short variance,
	const float eagerness) const {

	std::normal_distribution<double> xdist = std::normal_distribution<double>(-v.x, variance);
	std::normal_distribution<double> ydist = std::normal_distribution<double>(-v.y, variance);
	std::default_random_engine generator(seedTimer.elapsed()*1000000000.0f);
	return glm::vec2 {xdist(generator), ydist(generator)} * (float)pow(glm::length(v), eagerness);
}
