#include "removesystem.hpp"
#include <iostream>
#include "removecomponent.hpp"
#include "componentmanager.hpp"
#include "entitymanager.hpp"
#include <queue>

using std::cout;
using std::endl;

RemoveSystem::RemoveSystem(EntityManager* entityManager) :
    entityManager(entityManager) { }

void RemoveSystem::add(ID id) {
    ids.insert(id);
}
void RemoveSystem::remove(ID id) {
    if(ids.find(id) == ids.end()) {
        cout << "WARNING: Tried to remove unpresent ID from RemoveSystem. Anything can happen!" << endl;
        return;
    }
    ids.erase(id);
}
void RemoveSystem::update() {

    queue<ID> doomedIds;

    for(auto id : activeIds) {
        //1. If component have any last wishes, such as exploding or playing death sound
        //This would be the place to ensure that will happen

        //2. Kill the entity completely, not a single trace of it should exist beyond this point
        //(pushing it onto doomedIds to prevent iterator invalidations, it gets removed later)
        doomedIds.push(id);
    }
    while(!doomedIds.empty()) {
        entityManager->remove(doomedIds.front()); doomedIds.pop();
    }
}
unsigned int RemoveSystem::count() const {
    return ids.size();
}
const string RemoveSystem::getIdentifier() const {
    return "RemoveSystem";
}
void RemoveSystem::activateId(ID id) {
    if(ids.find(id) == ids.end()) {
        return;
    }

    activeIds.insert(id);
}
