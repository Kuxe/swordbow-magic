#include "componentmanager.h"
#include "movecomponent.h"
#include "rendercomponent.h"
#include "inputcomponent.h"
#include "tilecomponent.h"
#include "sizecomponent.h"
#include "namecomponent.h"
#include "moveeventcomponent.h"
#include <iostream>

using namespace std;

MoveComponent* ComponentManager::createMoveComponent(unsigned long long int* id) {
	moveComponents.insert(make_pair(id, MoveComponent()));
	return &moveComponents.at(id);
}
RenderComponent* ComponentManager::createRenderComponent(unsigned long long int* id) {
	renderComponents.insert(make_pair(id, RenderComponent()));
	return &renderComponents.at(id);
}

InputComponent* ComponentManager::createInputComponent(unsigned long long int* id) {
	inputComponents.insert(make_pair(id, InputComponent()));
	return &inputComponents.at(id);
}

TileComponent* ComponentManager::createTileComponent(unsigned long long int* id) {
	tileComponents.insert(make_pair(id, TileComponent()));
	return &tileComponents.at(id);
}

SizeComponent* ComponentManager::createSizeComponent(unsigned long long int* id) {
	sizeComponents.insert(make_pair(id, SizeComponent()));
	return &sizeComponents.at(id);
}

NameComponent* ComponentManager::createNameComponent(unsigned long long *id) {
	nameComponents.insert(make_pair(id, NameComponent()));
	return &nameComponents.at(id);
}

MoveEventComponent* ComponentManager::createMoveEventComponent(unsigned long long *id) {
	moveEventComponents.insert(make_pair(id, MoveEventComponent()));
	return &moveEventComponents.at(id);
}

PressKeyEventComponent* ComponentManager::createPressKeyEventComponent(unsigned long long *id) {
	pressKeyEventComponents.insert(make_pair(id, PressKeyEventComponent()));
	return &pressKeyEventComponents.at(id);
}

ReleaseKeyEventComponent* ComponentManager::createReleaseKeyEventComponent(unsigned long long *id) {
	releaseKeyEventComponents.insert(make_pair(id, ReleaseKeyEventComponent()));
	return &releaseKeyEventComponents.at(id);
}
