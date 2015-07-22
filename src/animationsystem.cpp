#include "animationsystem.hpp"
#include <iostream>
#include "componentmanager.hpp"
#include "systemmanager.hpp"
#include "client.hpp"
#include "socket.hpp"
#include "packet.hpp"
#include "messagetypes.hpp"

using std::cout;
using std::endl;

AnimationSystem::AnimationSystem(
    unordered_map<IpAddress, ClientData>* clients,
    Socket* socket) :
    clients(clients),
    socket(socket) { }

void AnimationSystem::add(ID id) {
    ids.insert(id);
    systemManager->getSystem(System::RENDERDIFF)->add(id);
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

        //create a unique number for each possible direction
        //if its an entity without direction, such as bloodsplatter, it will be 0
        //and neither switch-statement will occur.
		const char dirhash = mc.dir.x * 2 + mc.dir.y * 3;

		//If this entity is moving, play animation in correct direction
		if(glm::length(mc.vel) > 0) {
			switch(dirhash) {
				case -3: {
					ac.state = &ac.walking.north;
				} break;
				case -1: {
					ac.state = &ac.walking.northEast;
				} break;
				case 2: {
					ac.state = &ac.walking.east;
				} break;
				case 5: {
					ac.state = &ac.walking.southEast;
				} break;
				case 3: {
					ac.state = &ac.walking.south;
				} break;
				case 1: {
					ac.state = &ac.walking.southWest;
				} break;
				case -2: {
					ac.state = &ac.walking.west;
				} break;
				case -5: {
					ac.state = &ac.walking.northWest;
				} break;
			}
		}

		//Else this entity is standing still. Play idle animation in correct direction
		else {
			switch(dirhash) {
				case -3: {
					ac.state = &ac.idle.north;
				} break;
				case -1: {
					ac.state = &ac.idle.northEast;
				} break;
				case 2: {
					ac.state = &ac.idle.east;
				} break;
				case 5: {
					ac.state = &ac.idle.southEast;
				} break;
				case 3: {
					ac.state = &ac.idle.south;
				} break;
				case 1: {
					ac.state = &ac.idle.southWest;
				} break;
				case -2: {
					ac.state = &ac.idle.west;
				} break;
				case -5: {
					ac.state = &ac.idle.northWest;
				} break;
			}
		}

        auto& animation = ac.state;

        //Only play animation if it's a looping animation or the first time
        //playing the animation
        if(animation->looping || animation->firstLoop) {

            //Check if it's time to display the next frame
            const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - animation->startTime).count();
            if(elapsed > animation->duration) {
                auto& rc = componentManager->renderComponents.at(id);

                //It was time. Update the image associated with this entity
                rc.image = animation->frames[animation->currentFrame];

                //Update the animation
                animation->currentFrame += 1;
                animation->currentFrame %= animation->frames.size();
                animation->startTime = std::chrono::high_resolution_clock::now();

                //Frame 1, 2, 3, 4 ... n, 0 <- on that zero. First loop is done!
                if(animation->currentFrame == 0) {
                    animation->firstLoop = false;
                }

                //Since the rendercomponent changed, add this id to
                //RenderDiffSystem so that the network can send only the
                //changed rendercomponents. This implicitly activates
                //the ids of the components upon receival on the clients
                systemManager->getSystem(System::RENDERDIFF)->add(id);
            }
        }
    }
}
unsigned int AnimationSystem::count() const {
    return ids.size();
}
const System::Identifier AnimationSystem::getIdentifier() const {
    return System::ANIMATION;
}
void AnimationSystem::activateId(ID id) {
    //Why would this method be required for AnimationSystem?
}
