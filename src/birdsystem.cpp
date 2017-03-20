#include "birdsystem.hpp"
#include "componentmanager.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtc/matrix_access.hpp> //for glm::column()
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

	const auto getRandomAcceleration = [this](const glm::vec3& v, const unsigned short variance, const float eagerness) {
		std::normal_distribution<double> dist = std::normal_distribution<double>(0, variance);
		std::default_random_engine generator(seedTimer.elapsed()*1000000000.0f);
		return glm::vec3 {dist(generator) * -v.x, dist(generator) * -v.y, dist(generator) * -v.z} * (float)pow(glm::length(v), eagerness);
	};

	for(const auto& id : swarmIds) {
		//TODO: fixedPoint could be data in a swarmcomponent, right?§
		const auto& fixedPoint = fixedPoints.at(id);
		const auto& mc = componentManager->moveComponents.at(id);
		auto& ac = componentManager->accelerationComponents.at(id);
		const glm::vec3 pos = glm::vec3(glm::column(mc.transform, 3));
		ac.acceleration = ac.acceleration*0.9995f + getRandomAcceleration(pos - fixedPoint, 2000, 0.001f);
	}

	for(auto id : birdIds) {
		const auto& birdComponent = componentManager->birdComponents.at(id);
		const auto& mc = componentManager->moveComponents.at(id);
		const auto& swarmId = birdComponent.swarmId;
		const auto& swarmPoint = glm::vec3(glm::column(componentManager->moveComponents.at(swarmId).transform, 3));
		auto& ac = componentManager->accelerationComponents.at(id);
		const glm::vec3 pos = glm::vec3(glm::column(mc.transform, 3));
		ac.acceleration = ac.acceleration*0.995f + getRandomAcceleration(pos - swarmPoint, 333, 0.01f);
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
