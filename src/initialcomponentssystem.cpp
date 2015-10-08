#include "initialcomponentssystem.hpp"
#include "componentmanager.hpp"

void InitialComponentsSystem::add(ID id) {
	ids.insert(id);
}

void InitialComponentsSystem::remove(ID id) {
	ids.erase(id);
}

void InitialComponentsSystem::update() {
	
}

unsigned int InitialComponentsSystem::count() const {
	return ids.size();
}

const System::Identifier InitialComponentsSystem::getIdentifier() const {
	return System::INITIAL_COMPONENTS;
}

bool InitialComponentsSystem::activateId(ID id) {
	return false;
}