#include "removesystem.hpp"
#include "componentmanager.hpp"
#include "entitymanager.hpp"
#include "icommand.hpp"

/** For logging **/
#include "logger.hpp"

RemoveSystem::RemoveSystem(EntityManager* entityManager) :
    entityManager(entityManager) { }

void RemoveSystem::add(ID id) {
    ids.insert(id);
}
void RemoveSystem::remove(ID id) {
    if(ids.find(id) == ids.end()) {
        Logger::log("Tried to remove unpresent ID from RemoveSystem. Anything can happen!", Log::WARNING);
        return;
    }
    ids.erase(id);
}
void RemoveSystem::update() {
    while(!activeIds.empty()) {
        auto id = activeIds.front(); activeIds.pop();
        auto& cc = componentManager->commandComponents.at(id);

        //1. If component have any last wishes, such as exploding or playing death sound
        cc.execute(CommandComponent::Event::ON_DEATH);

        //2. Kill the entity completely, not a single trace of it should exist beyond this point
        //(pushing it onto doomedIds to prevent iterator invalidations, it gets removed later)
        entityManager->remove(id);
    }
}
unsigned int RemoveSystem::count() const {
    return ids.size();
}
const System::Identifier RemoveSystem::getIdentifier() const {
    return System::REMOVE;
}
bool RemoveSystem::activateId(ID id) {
    if(ids.find(id) != ids.end()) {
        activeIds.push(id);
        return true;
    }
    return false;
}
