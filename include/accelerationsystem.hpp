#ifndef ACCELERATIONSYSTEM_HPP
#define ACCELERATIONSYSTEM_HPP

#include "isystem.hpp"
#include "deltatime.hpp"
#include <unordered_set>

typedef unsigned int ID;

class AccelerationSystem : ISystem {
private:
	DeltaTime deltaTime;
	std::unordered_set<ID> ids;

public:

	AccelerationSystem(const DeltaTime& deltaTime);

	void add(ID id);
 	void remove(ID id);
	void update();
	unsigned int count() const;
	const System::Identifier getIdentifier() const;
    bool activateId(ID id);
};

#endif //ACCELERATIONSYSTEM_HPP