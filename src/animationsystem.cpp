#include "animationsystem.hpp"
#include <iostream>

using std::cout;
using std::endl;

void AnimationSystem::add(ID id) {
    ids.insert(id);
}
void AnimationSystem::remove(ID id) {
    if(ids.find(id) == ids.end()) {
        cout << "ERROR: Tried to remove unpresent ID from AnimationSystem. Don't know what will happen from now on!" << endl;
        return;
    }
    ids.erase(id);
}
void AnimationSystem::update() {
    //What do I do here?
}
unsigned int AnimationSystem::count() const {
    return ids.size();
}
const string AnimationSystem::getIdentifier() const {
    return "AnimationSystem";
}
void AnimationSystem::activateId(ID id) {
    //What do I do here?
}
