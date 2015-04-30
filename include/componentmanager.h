#ifndef COMPONENTMANAGER_H
#define COMPONENTMANAGER_H

#include <unordered_map>
#include "movecomponent.h"
#include "rendercomponent.h"
#include "inputcomponent.h"
#include "tilecomponent.h"
#include "sizecomponent.h"
#include "namecomponent.h"

using namespace std;

class SizeComponent;
class NameComponent;

class ComponentManager {
 public:
	unordered_map<unsigned long long int *, MoveComponent> moveComponents;
	unordered_map<unsigned long long int *, RenderComponent> renderComponents;
	unordered_map<unsigned long long int *, InputComponent> inputComponents;
	unordered_map<unsigned long long int *, TileComponent> tileComponents;
	unordered_map<unsigned long long int *, SizeComponent> sizeComponents;
    unordered_map<unsigned long long int *, NameComponent> nameComponents;

	MoveComponent* createMoveComponent(unsigned long long int* id);
	RenderComponent* createRenderComponent(unsigned long long int* id);
	InputComponent* createInputComponent(unsigned long long int* id);
	TileComponent* createTileComponent(unsigned long long int* id);
	SizeComponent* createSizeComponent(unsigned long long int* id);
    NameComponent* createNameComponent(unsigned long long int* id);
};

#endif //COMPONENTMANAGER_H
