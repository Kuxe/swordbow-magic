#include "removesystem.hpp"
#include <iostream>
#include "removecomponent.hpp"
#include "componentmanager.hpp"
#include "entitymanager.hpp"
#include <queue>
#include "systemmanager.hpp"

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
    while(!activeIds.empty()) {
        auto id = activeIds.front(); activeIds.pop();

        //1. If component have any last wishes, such as exploding or playing death sound
        //This would be the place to ensure that will happen

        //Make sure entity disappears visually on removal
        auto spatialIndexer = dynamic_cast<SpatialIndexer*>(systemManager->getSystem("TextureHashGridSystem"));
        static_cast<RenderSystem*>(systemManager->getSystem("RenderSystem"))->renderArea(spatialIndexer->getBoundingBox(id));

        //Execute whatever last wishes the entity has
        for(auto command : componentManager->commandComponents.at(id)[6]) {
            command->execute();
        }

        //2. Kill the entity completely, not a single trace of it should exist beyond this point
        //(pushing it onto doomedIds to prevent iterator invalidations, it gets removed later)
        entityManager->remove(id);
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

    activeIds.push(id);
}
