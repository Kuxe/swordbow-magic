#ifndef COMPONENTMANAGER_H
#define COMPONENTMANAGER_H

#include <unordered_map>
#include "movecomponent.hpp"
#include "rendercomponent.hpp"
#include "inputcomponent.hpp"
#include "tilecomponent.hpp"
#include "sizecomponent.hpp"
#include "namecomponent.hpp"
#include "moveeventcomponent.hpp"
#include "presskeyeventcomponent.hpp"
#include "releasekeyeventcomponent.hpp"

using namespace std;

class ComponentManager {
 public:
	unordered_map<unsigned long long int *, MoveComponent> moveComponents;
	unordered_map<unsigned long long int *, RenderComponent> renderComponents;
	unordered_map<unsigned long long int *, InputComponent> inputComponents;
	unordered_map<unsigned long long int *, TileComponent> tileComponents;
	unordered_map<unsigned long long int *, SizeComponent> sizeComponents;
    unordered_map<unsigned long long int *, NameComponent> nameComponents;
    unordered_map<unsigned long long int *, MoveEventComponent> moveEventComponents;
    unordered_map<unsigned long long int *, PressKeyEventComponent> pressKeyEventComponents;
    unordered_map<unsigned long long int *, ReleaseKeyEventComponent> releaseKeyEventComponents;

	MoveComponent* createMoveComponent(unsigned long long int* id);
	RenderComponent* createRenderComponent(unsigned long long int* id);
	InputComponent* createInputComponent(unsigned long long int* id);
	TileComponent* createTileComponent(unsigned long long int* id);
	SizeComponent* createSizeComponent(unsigned long long int* id);
    NameComponent* createNameComponent(unsigned long long int* id);
    MoveEventComponent* createMoveEventComponent(unsigned long long int* id);
    PressKeyEventComponent* createPressKeyEventComponent(unsigned long long int* id);
    ReleaseKeyEventComponent* createReleaseKeyEventComponent(unsigned long long int* id);
};

#endif //COMPONENTMANAGER_H
