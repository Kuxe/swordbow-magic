#ifndef MOVEDIFFSYSTEM_HPP
#define MOVEDIFFSYSTEM_HPP

#include "isystem.hpp"
#include <unordered_set>

typedef unsigned int ID;

/** Adding to this system will make sure
	that the movecomponent of added ID will
	be sent to clients

	After server sent the movecomponents
	all ids is erased from this system
**/
class MoveDiffSystem : public ISystem {
    std::unordered_set<ID> ids;
public:
    void add(ID id);
    void remove(ID id);
    void update();
    unsigned int count() const;
    const System::Identifier getIdentifier() const;
    void activateId(ID id);

    auto begin() const { return ids.begin(); }
    auto end() const { return ids.end(); } 
};

#endif //MOVEDIFFSYSTEM_HPP
