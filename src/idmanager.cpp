#include "idmanager.hpp"

/** For logging **/
#include "logger.hpp"
#include <ostream>

IdManager::IdManager() {
	std::ostringstream oss;
	oss << "Available IDs: " << MAX_IDS << ", IdManager memory footprint: " << MAX_IDS * sizeof(ID) / 1048576 << "MB";
	Logger::log(oss, Log::INFO);
}

unsigned int IdManager::acquireId() {

	//If any ID has been returned, reuse it
	if(!freeIds.empty()) {
		const ID recycledId = freeIds.top(); freeIds.pop();
		return recycledId;
	}

	//Else get next free id;
	static ID id = 0;
	id += 1;
	if(id == MAX_IDS) {
		Logger::log("Ran out of free ids (idmanager.cpp). Either increase the amount of freeIds or reduce entities in world", Log::ERROR);
	}
	return id;
}

void IdManager::releaseId(unsigned int releasedId) {
	freeIds.push(releasedId);
}
