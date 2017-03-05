#include "attacksystem.hpp"
#include "hashgridsystem.hpp"
#include "componentmanager.hpp"
#include "sounddata.hpp"

/** For network **/
#include "packetmanager.hpp"
#include "packet.hpp"
#include "messagetypes.hpp"

/** For logging **/
#include "logger.hpp"

AttackSystem::AttackSystem(
    HashGridSystem* hashgrid,
    std::unordered_map<IpAddress, ClientData>* clients,
    PacketManager* packetManager) :
    hashgrid(hashgrid),
    clients(clients),
    packetManager(packetManager) {

}

void AttackSystem::add(ID id) {
    ids.insert(id);
}
void AttackSystem::remove(ID id) {
    if(ids.find(id) == ids.end()) {
        Logger::log("Tried to remove unpresent id from AttackSystem", Logger::WARNING);
    }
    ids.erase(id);
}
void AttackSystem::update() {
    while(!activeIds.empty()) {
        auto id = activeIds.front(); activeIds.pop();
        auto& ac = componentManager->attackComponents.at(id);
        auto& mc = componentManager->moveComponents.at(id);

        //The id is active, that means it attacked.
        //First, calculate what the absolute impact area is
        //from the relative impact area, ie the area that
        //the entity attacked taking the entities position into account

        //Calculate impactarea of a blow/strike
        //In words: Draw a vector of length 'weaponLength' from the center of the
        //entity in it's facing direction. Place a box with the size of ac.impactArea
        //with the tip of the vector as the box's center

        // TODO: Reimplement from 2D to 3D
        /*
        float x = mc.pos.x + ac.impactArea.x + mc.dir.x*ac.weaponLength - ac.impactArea.w/2;
        float y = mc.pos.y + ac.impactArea.y + mc.dir.y*ac.weaponLength - ac.impactArea.h/2;
        if(x < 0 || y < 0) return; //If someone tries to strike outside of world, disallow the strike

        const Rect impactArea {
            x,
            y,
            ac.impactArea.w,
            ac.impactArea.h };

        //So the entity probably attacked within some area
        //Query that area, for all entities that can be damaged
        //within that area (IE all entities with a healthcomponent),
        //apply the damage to their healthcomponent

        //First of all, check that sufficient amount of time has bypassed
        //since last attack...
        const auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - ac.startTime).count();
        if(diff > ac.delay) {
            ac.startTime = std::chrono::high_resolution_clock::now();

            //Will be set to true if something was actually hit
            //Is checked when playing a default 'hurt' sound
            bool somethingWasHurt = false;

            for(auto attackedId : hashgrid->query(impactArea)) {

                //Only apply damage to entities that has a healthcomponent
                auto hcit = componentManager->healthComponents.find(attackedId);
                if(hcit != componentManager->healthComponents.end()) {
                    auto& hc = std::get<1>(*hcit);
                    hc.health -= ac.damage;
                    somethingWasHurt = true;
                }
            }

            if(somethingWasHurt) {
                //Play a default 'hurt'-sound for
                SoundData hurtSound = {Sound::HURT};

                //Broadcast hurtsound to all clients
                for(auto& pair : *clients) {
                    auto& clientData = pair.second;
                    auto packet = Packet<SoundData, MESSAGE_TYPE::PLAY_SOUND> {
                		stringhash("swordbow-magic"),
                        clientData.sequence++,
                		hurtSound,
                		sizeof(hurtSound)
                	};
                	packetManager->send(pair.first, packet);
                }
            }
        }
    */
    }
}
unsigned int AttackSystem::count() const {
    return ids.size();
}
const System::Identifier AttackSystem::getIdentifier() const {
    return System::ATTACK;
}
bool AttackSystem::activateId(ID id) {
    if(ids.find(id) != ids.end()) {
        activeIds.push(id);
        return true;
    }
    return false;
}
