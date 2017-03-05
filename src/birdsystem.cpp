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
		const auto& mc = componentManager->moveComponents.at(id);
		auto& ac = componentManager->accelerationComponents.at(id);
		//TODO: Convert from 2D to 3D ac.acceleration = ac.acceleration*0.9995f + getRandomAcceleration(mc.pos - fixedPoint, 2000, 0.001f);
	}

	for(auto id : birdIds) {
		const auto& birdComponent = componentManager->birdComponents.at(id);
		const auto& mc = componentManager->moveComponents.at(id);
		const auto& swarmId = birdComponent.swarmId;
		//TODO: Convert from 2D to 3D const auto& swarmPoint = componentManager->moveComponents.at(swarmId).pos;
		auto& ac = componentManager->accelerationComponents.at(id);
		//TODO: Convert from 2D to 3D ac.acceleration = ac.acceleration*0.995f + getRandomAcceleration(mc.pos - swarmPoint, 333, 0.01f);
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

ID BirdSystem::createSwarm(const glm::vec3& fixedPoint) {
	//TODO: Generate swarmPoint
	glm::mat3 transform = glm::mat3();
	auto swarmId = entityManager->createSwarmPoint(transform);
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

glm::vec3 BirdSystem::getRandomAcceleration(
	const glm::vec3& v,
	const unsigned short variance,
	const float eagerness) const {

	std::normal_distribution<double> xdist = std::normal_distribution<double>(-v.x, variance);
	std::normal_distribution<double> ydist = std::normal_distribution<double>(-v.y, variance);
	std::normal_distribution<double> zdist = std::normal_distribution<double>(-v.z, variance);
	std::default_random_engine generator(seedTimer.elapsed()*1000000000.0f);
	return glm::vec3 {xdist(generator), ydist(generator), zdist(generator)} * (float)pow(glm::length(v), eagerness);
}
