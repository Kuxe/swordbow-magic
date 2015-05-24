#include "animationsystem.hpp"
#include <iostream>
#include "componentmanager.hpp"
#include "systemmanager.hpp"

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
    for(auto id : ids) {
        auto& ac = componentManager->animationComponents.at(id);
        auto& animation = ac.state;

        //Only play animation if it's a looping animation or the first time
        //playing the animation
        if(animation->looping || animation->firstLoop) {

            //Check if it's time to display the next frame
            if(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - animation->startTime).count() > animation->duration) {
                auto rs = static_cast<RenderSystem*>(systemManager->getSystem("RenderSystem"));

                //It was time. Update the image associated with this entity
                rs->setImage(id, animation->frames[animation->currentFrame]);

                //Update the animation
                animation->currentFrame += 1;
                animation->currentFrame %= animation->frames.size();
                animation->startTime = std::chrono::high_resolution_clock::now();

                //Frame 1, 2, 3, 4 ... n, 0 <- on that zero. First loop is done!
                if(animation->currentFrame == 0) {
                    animation->firstLoop = false;
                }
            }
        }
    }
}
unsigned int AnimationSystem::count() const {
    return ids.size();
}
const string AnimationSystem::getIdentifier() const {
    return "AnimationSystem";
}
void AnimationSystem::activateId(ID id) {
    //Why would this method be required for AnimationSystem?
}
