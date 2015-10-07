#ifndef BIRDSYSTEM_HPP
#define BIRDSYSTEM_HPP

#include "isystem.hpp"
#include <glm/vec2.hpp>
#include <unordered_set>
#include <unordered_map>
#include "idmanager.hpp"
#include <random>
#include "timer.hpp"

typedef unsigned int ID;
typedef unsigned char SwarmIndex;

class BirdSystem : public ISystem {
private:
	std::unordered_set<ID> ids;
	std::unordered_map<SwarmIndex, glm::vec2> swarmPoints;
	std::unordered_map<SwarmIndex, glm::vec2> fixedPoints;
	IdManager idManager;
	Timer seedTimer;

	glm::vec2 getRandomAcceleration(
		const glm::vec2& vec1,
		const glm::vec2& vec2,
		const unsigned short variance) const;

public:
	BirdSystem();

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
    unsigned char createSwarm(const glm::vec2& fixedPoint);

    //Swarm won't any longer affect birds in swarm 
    bool dissolveSwarm(SwarmIndex swarmIndex);

};

#endif //BIRDSYSTEM_HPP