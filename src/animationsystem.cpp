#include "animationsystem.hpp"
#include <iostream>
#include "componentmanager.hpp"
#include "systemmanager.hpp"

using std::cout;
using std::endl;

void AnimationSystem::add(ID id) {
    ids.insert(id);

    //Default state to idleSouth
    auto& ac = componentManager->animationComponents.at(id);
    ac.state = &ac.idle.south;
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
        auto& mc = componentManager->moveComponents.at(id);

        //update state by checking in what direction entity is moving
		auto xdir = mc.pos.x - mc.oldPos.x; //if > 0 Moved to right
		auto ydir = mc.pos.y - mc.oldPos.y; //if > 0 Moved to down
		if(xdir > 0 && ydir > 0) {
            ac.state = &ac.walking.southEast;
        } else if(xdir > 0 && ydir == 0) {
            ac.state = &ac.walking.east;
        } else if(xdir > 0 && ydir < 0) {
            ac.state = &ac.walking.northEast;
        } else if(xdir == 0 && ydir > 0) {
            ac.state = &ac.walking.south;
        } else if(xdir == 0 && ydir == 0) {

            //If entity was walking in a certain direction,
            //then the entitys idle direction should be in
            //whatever the walking direction was
            if(ac.state == &ac.walking.north) {
                ac.state = &ac.idle.north;
            } else if (ac.state == &ac.walking.northEast) {
                ac.state = &ac.idle.northEast;
            } else if (ac.state == &ac.walking.east) {
                ac.state = &ac.idle.east;
            } else if (ac.state == &ac.walking.southEast) {
                ac.state = &ac.idle.southEast;
            } else if (ac.state == &ac.walking.south) {
                ac.state = &ac.idle.south;
            } else if (ac.state == &ac.walking.southWest) {
                ac.state = &ac.idle.southWest;
            } else if (ac.state == &ac.walking.west) {
                ac.state = &ac.idle.west;
            } else if (ac.state == &ac.walking.northWest) {
                ac.state = &ac.idle.northWest;
            }
        } else if(xdir == 0 && ydir < 0) {
            ac.state = &ac.walking.north;
        } else if(xdir < 0 && ydir > 0) {
            ac.state = &ac.walking.southWest;
        } else if(xdir < 0 && ydir == 0) {
            ac.state = &ac.walking.west;
        } else if(xdir < 0 && ydir < 0) {
            ac.state = &ac.walking.northWest;
        }

        updateAnimation(id, ac.state);
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

void AnimationSystem::updateAnimation(ID id, AnimationComponent::Animation* const animation) const {
    //Check if it's time to display the next frame
    if(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - animation->startTime).count() > animation->duration) {
        auto rs = static_cast<RenderSystem*>(systemManager->getSystem("RenderSystem"));

        //It was time. Update the image associated with this entity
        rs->setImage(id, animation->frames[animation->currentFrame]);

        //Update the animation
        animation->currentFrame += 1;
        animation->currentFrame %= animation->frames.size();
        animation->startTime = std::chrono::high_resolution_clock::now();
    }
}
