#include "componentmanager.h"
#include "movecomponent.h"
#include "rendercomponent.h"
#include "inputcomponent.h"
#include "tilecomponent.h"
#include "flagcomponent.h"
#include <iostream>

using namespace std;

MoveComponent* ComponentManager::createMoveComponent(unsigned long long int* id) {
	MoveComponent* moveComponent = new MoveComponent();
	moveComponents.insert(make_pair(id, moveComponent));
	return moveComponent;
}
RenderComponent* ComponentManager::createRenderComponent(unsigned long long int* id) {
	RenderComponent* renderComponent = new RenderComponent();
	renderComponents.insert(make_pair(id, renderComponent));
	return renderComponent;
}

InputComponent* ComponentManager::createInputComponent(unsigned long long int* id) {
	InputComponent* inputComponent = new InputComponent();
	inputComponents.insert(make_pair(id, inputComponent));
	return inputComponent;
}

TileComponent* ComponentManager::createTileComponent(unsigned long long int* id) {
	TileComponent* tileComponent = new TileComponent();
	tileComponents.insert(make_pair(id, tileComponent));
	return tileComponent;
}

FlagComponent* ComponentManager::createFlagComponent(unsigned long long int* id) {
	FlagComponent* flagComponent = new FlagComponent();
	flagComponents.insert(make_pair(id, flagComponent));
	return flagComponent;
}

void ComponentManager::removeAllComponents(unsigned long long int* id) {
	try {
		delete moveComponents.at(id);
	} catch (const std::out_of_range& oor) {
		cout << "id has no MoveComponent" << endl;
	}

	try {
		delete renderComponents.at(id);
	} catch (const std::out_of_range& oor) {
		cout << "id has no renderComponents" << endl;
	}

	try {
		delete inputComponents.at(id);
	} catch (const std::out_of_range& oor) {
		cout << "id has no inputComponents" << endl;
	}

	try {
		delete tileComponents.at(id);
	} catch (const std::out_of_range& oor) {
		cout << "id has no tileComponents" << endl;
	}

	try {
		delete flagComponents.at(id);
	} catch (const std::out_of_range& oor) {
		cout << "id has no flagComponents" << endl;
	}
}













