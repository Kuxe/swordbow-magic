#ifndef BIRDSYSTEM_HPP
#define BIRDSYSTEM_HPP

#include "isystem.hpp"
#include <glm/vec2.hpp>
#include <unordered_set>

typedef unsigned int ID;

class BirdSystem : public ISystem {
private:
	std::unordered_set<ID> ids;
	glm::vec2 fixedPoint = {0, 0};
	glm::vec2 swarmPoint;

public:
	void add(ID id);
 	void remove(ID id);
	void update();
	unsigned int count() const;
	const System::Identifier getIdentifier() const;
    bool activateId(ID id);

    void setFixedPoint(const glm::vec2& fixedPoint);
};

#endif //BIRDSYSTEM_HPP