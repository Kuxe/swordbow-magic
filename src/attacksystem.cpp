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
        // TODO: Implement
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
