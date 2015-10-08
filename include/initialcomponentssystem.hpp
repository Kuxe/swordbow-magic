#ifndef INTITIALCOMPONENTSSYSTEM_HPP
#define INTITIALCOMPONENTSSYSTEM_HPP

#include "isystem.hpp"
#include <unordered_set>

/** If any entity is member of this system, it will be
	sent in the INITIAL_COMPONENTS that is sent to clients
	upon connecting to the server
**/
class InitialComponentsSystem : public ISystem {
private:
	std::unordered_set<ID> ids;

public:
	void add(ID id);
 	void remove(ID id);
	void update();
	unsigned int count() const;
	const System::Identifier getIdentifier() const;
    bool activateId(ID id);

    auto begin() const { return ids.begin(); }
    auto end() const { return ids.end(); }
};


#endif //INTITIALCOMPONENTSSYSTEM_HPP