#include "initialcomponentssystem.hpp"
#include "componentmanager.hpp"

void InitialComponentsSystem::add(ID id) {
	ids.insert(id);
	initialComponents.first.insert({id, componentManager->moveComponents.at(id)});
	initialComponents.second.insert({id, componentManager->renderComponents.at(id)});
}

void InitialComponentsSystem::remove(ID id) {
	ids.erase(id);
	initialComponents.first.erase(id);
	initialComponents.first.erase(id);
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