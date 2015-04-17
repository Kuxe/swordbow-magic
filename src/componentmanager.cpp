#include "componentmanager.h"
#include "movecomponent.h"
#include "rendercomponent.h"
#include "inputcomponent.h"
#include "tilecomponent.h"
#include "sizecomponent.h"
#include "namecomponent.h"
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

SizeComponent* ComponentManager::createSizeComponent(unsigned long long int* id) {
	SizeComponent* sizeComponent = new SizeComponent();
	sizeComponents.insert(make_pair(id, sizeComponent));
	return sizeComponent;
}

NameComponent* ComponentManager::createNameComponent(unsigned long long *id) {
	NameComponent* nameComponent = new NameComponent();
	nameComponents.insert(make_pair(id, nameComponent));
	return nameComponent;
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
		delete sizeComponents.at(id);
	} catch (const std::out_of_range& oor) {
		cout << "id has no sizeComponents" << endl;
	}

	try {
		delete nameComponents.at(id);
	} catch (const std::out_of_range& oor) {
		cout << "id has no nameComponent" << endl;
	}
}
