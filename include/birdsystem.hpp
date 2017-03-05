#ifndef BIRDSYSTEM_HPP
#define BIRDSYSTEM_HPP

#include "isystem.hpp"
#include <glm/vec3.hpp>
#include <unordered_set>
#include <unordered_map>
#include <random>
#include "timer.hpp"

typedef unsigned int ID;

class EntityManager;

class BirdSystem : public ISystem {
private:
	std::unordered_set<ID> birdIds;
	std::unordered_set<ID> swarmIds;
	std::unordered_map<ID, glm::vec3> fixedPoints;
	Timer seedTimer;

	EntityManager* const entityManager;

	glm::vec3 getRandomAcceleration(
		const glm::vec3& v,
		const unsigned short variance,
		const float eagerness) const;

public:
	BirdSystem(EntityManager* const entityManager);

	void add(ID id);
 	void remove(ID id);
	void update();
	unsigned int count() const;
	const System::Identifier getIdentifier() const;
    bool activateId(ID id);

    //Creates a swarm on circulating the fixedPoints.
    //For any entity to be attached to the swarm, the entity
    //needs a birdComponent with swarmIndex = createSwarm({x, y});
    //For several birds to be part of a swarm, give them the same swarmIndex
    ID createSwarm(const glm::vec3& fixedPoint);

    //Swarm won't any longer affect birds in swarm 
    bool dissolveSwarm(ID swarmId);

};

#endif //BIRDSYSTEM_HPP