#include "idmanager.hpp"
#include <iostream>

using namespace std;

IdManager::IdManager() {
	for(unsigned int freeId = MAX_IDS-1; freeId > 0; freeId--) {
		freeIds.push(freeId);
	}
}

unsigned int IdManager::acquireId() {
	if(freeIds.empty()) {
		cout << "Error: Ran out of free ids (idmanager.cpp). Either increase the amount of freeIds or reduce entities in world." << endl;
	}
	const auto acquiredId = freeIds.top(); freeIds.pop();
	return acquiredId;
}

void IdManager::releaseId(unsigned int releasedId) {
	freeIds.push(releasedId);
}
