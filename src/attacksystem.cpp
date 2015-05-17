#include "attacksystem.hpp"
#include "hashgridsystem.hpp"
#include "componentmanager.hpp"

#include <iostream>
using namespace std;

AttackSystem::AttackSystem(HashGridSystem* hashgrid) :
    hashgrid(hashgrid) {

}

void AttackSystem::add(ID id) {
    ids.insert(id);
}
void AttackSystem::remove(ID id) {
    if(ids.find(id) == ids.end()) {
        cout << "WARNING: Tried to remove unpresent ID from AttackSystem!" << endl;
    }
    ids.erase(id);
}
void AttackSystem::update() {
    while(!activeIds.empty()) {
        auto id = activeIds.front(); activeIds.pop();
        auto& ac = componentManager->attackComponents.at(id);

        //The id is active, that means it attacked.
        //First, calculate what the absolute impact area is
        //from the relative impact area, ie the area that
        //the entity attacked taking the entities position into account

        //TODO: This should (of course) depend on position of entity
        //currently theres just damage-blow to everything within a
        //box in top the left corner...
        SpatialIndexer::Rect impactArea {10, 10, 10, 10};

        //So the entity probably attacked within some area
        //Query that area, for all entities that can be damaged
        //within that area (IE all entities with a healthcomponent),
        //apply the damage to their healthcomponent

        //First of all, check that sufficient amount of time has bypassed
        //since last attack...
        if(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - ac.startTime).count() > ac.delay) {

            //... if so, apply attack damage on the impactarea
            for(auto attackedId : hashgrid->query(impactArea)) {
                auto& hc = componentManager->healthComponents.at(attackedId); //FIXME: What happens if theres no healthcomponent at id?
                hc.health -= ac.damage;
            }
        }

    }
}
unsigned int AttackSystem::count() const {
    return ids.size();
}
const string AttackSystem::getIdentifier() const {
    return "AttackSystem";
}
void AttackSystem::activateId(ID id) {
    if(ids.find(id) == ids.end()) {
        return;
    }
    activeIds.push(id);
}
