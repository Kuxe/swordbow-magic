#include "animationsystem.hpp"
#include "animationcomponent.hpp"
#include "logger.hpp"
#include "componentmanager.hpp"
#include "systemmanager.hpp"

void AnimationSystem::add(ID id) {
    ids.insert(id);
    systemManager->getSystem(System::RENDERDIFF)->add(id);
}
void AnimationSystem::remove(ID id) {
    if(ids.find(id) == ids.end()) {
        Logger::log("Tried to remove unpresent ID from AnimationSystem. Don't know what will happen from now on!", Logger::ERROR);
        return;
    }
    ids.erase(id);
}

void AnimationSystem::update() {
    for(auto id : ids) {
        auto& ac = componentManager->animationComponents.at(id);
        auto& mc = componentManager->moveComponents.at(id);
        //TODO: Implement
    }
}
unsigned int AnimationSystem::count() const {
    return ids.size();
}
const System::Identifier AnimationSystem::getIdentifier() const {
    return System::ANIMATION;
}
bool AnimationSystem::activateId(ID id) {
    //Why would this method be required for AnimationSystem?
    return false;
}
