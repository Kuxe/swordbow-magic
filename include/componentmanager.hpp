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
#include "soundcomponent.hpp"

using namespace std;

typedef unsigned int ID;

class ComponentManager {
 public:
	unordered_map<ID, MoveComponent> moveComponents;
	unordered_map<ID, RenderComponent> renderComponents;
	unordered_map<ID, InputComponent> inputComponents;
	unordered_map<ID, TileComponent> tileComponents;
	unordered_map<ID, SizeComponent> sizeComponents;
    unordered_map<ID, NameComponent> nameComponents;
    unordered_map<ID, MoveEventComponent> moveEventComponents;
    unordered_map<ID, PressKeyEventComponent> pressKeyEventComponents;
    unordered_map<ID, ReleaseKeyEventComponent> releaseKeyEventComponents;
    unordered_map<ID, SoundComponent> soundComponents;

	MoveComponent* createMoveComponent(ID id);
	RenderComponent* createRenderComponent(ID id);
	InputComponent* createInputComponent(ID id);
	TileComponent* createTileComponent(ID id);
	SizeComponent* createSizeComponent(ID id);
    NameComponent* createNameComponent(ID id);
    MoveEventComponent* createMoveEventComponent(ID id);
    PressKeyEventComponent* createPressKeyEventComponent(ID id);
    ReleaseKeyEventComponent* createReleaseKeyEventComponent(ID id);
    SoundComponent* createSoundComponent(ID id);
};

#endif //COMPONENTMANAGER_H
