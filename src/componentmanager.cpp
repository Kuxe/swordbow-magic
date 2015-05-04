#include "componentmanager.hpp"
#include "movecomponent.hpp"
#include "rendercomponent.hpp"
#include "inputcomponent.hpp"
#include "tilecomponent.hpp"
#include "sizecomponent.hpp"
#include "namecomponent.hpp"
#include "moveeventcomponent.hpp"
#include <iostream>

using namespace std;

MoveComponent* ComponentManager::createMoveComponent(ID id) {
	moveComponents.insert(make_pair(id, MoveComponent()));
	return &moveComponents.at(id);
}
RenderComponent* ComponentManager::createRenderComponent(ID id) {
	renderComponents.insert(make_pair(id, RenderComponent()));
	return &renderComponents.at(id);
}

InputComponent* ComponentManager::createInputComponent(ID id) {
	inputComponents.insert(make_pair(id, InputComponent()));
	return &inputComponents.at(id);
}

TileComponent* ComponentManager::createTileComponent(ID id) {
	tileComponents.insert(make_pair(id, TileComponent()));
	return &tileComponents.at(id);
}

SizeComponent* ComponentManager::createSizeComponent(ID id) {
	sizeComponents.insert(make_pair(id, SizeComponent()));
	return &sizeComponents.at(id);
}

NameComponent* ComponentManager::createNameComponent(ID id) {
	nameComponents.insert(make_pair(id, NameComponent()));
	return &nameComponents.at(id);
}

MoveEventComponent* ComponentManager::createMoveEventComponent(ID id) {
	moveEventComponents.insert(make_pair(id, MoveEventComponent()));
	return &moveEventComponents.at(id);
}

PressKeyEventComponent* ComponentManager::createPressKeyEventComponent(ID id) {
	pressKeyEventComponents.insert(make_pair(id, PressKeyEventComponent()));
	return &pressKeyEventComponents.at(id);
}

ReleaseKeyEventComponent* ComponentManager::createReleaseKeyEventComponent(ID id) {
	releaseKeyEventComponents.insert(make_pair(id, ReleaseKeyEventComponent()));
	return &releaseKeyEventComponents.at(id);
}
