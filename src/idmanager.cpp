#include "idmanager.hpp"
#include <iostream>

IdManager::IdManager() {
	std::cout << "Available IDs: " << MAX_IDS << ", IdManager memory footprint: " << MAX_IDS * sizeof(ID) / 1048576 << "MB" << std::endl;
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
			std::cout << "Error: Ran out of free ids (idmanager.cpp). Either increase the amount of freeIds or reduce entities in world." << std::endl;
	}
	return id;
}

void IdManager::releaseId(unsigned int releasedId) {
	freeIds.push(releasedId);
}
