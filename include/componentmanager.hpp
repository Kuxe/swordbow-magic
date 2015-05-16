#ifndef COMPONENTMANAGER_H
#define COMPONENTMANAGER_H

#include <unordered_map>
#include "animationcomponent.hpp"
#include "movecomponent.hpp"
#include "rendercomponent.hpp"
#include "inputcomponent.hpp"
#include "tilecomponent.hpp"
#include "sizecomponent.hpp"
#include "namecomponent.hpp"
#include "moveeventcomponent.hpp"
#include "releasekeyeventcomponent.hpp"
#include "soundcomponent.hpp"
#include "healthcomponent.hpp"
#include "removecomponent.hpp"
#include "attackcomponent.hpp"
#include "commandcomponent.hpp"

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
    unordered_map<ID, ReleaseKeyEventComponent> releaseKeyEventComponents;
    unordered_map<ID, SoundComponent> soundComponents;
    unordered_map<ID, AnimationComponent> animationComponents;
    unordered_map<ID, HealthComponent> healthComponents;
    unordered_map<ID, RemoveComponent> removeComponents;
    unordered_map<ID, AttackComponent> attackComponents;
    unordered_map<ID, CommandComponent> commandComponents;

	MoveComponent* createMoveComponent(ID id);
	RenderComponent* createRenderComponent(ID id);
	InputComponent* createInputComponent(ID id);
	TileComponent* createTileComponent(ID id);
	SizeComponent* createSizeComponent(ID id);
    NameComponent* createNameComponent(ID id);
    MoveEventComponent* createMoveEventComponent(ID id);
    ReleaseKeyEventComponent* createReleaseKeyEventComponent(ID id);
    SoundComponent* createSoundComponent(ID id);
    AnimationComponent* createAnimationComponent(ID id);
    HealthComponent* createHealthComponent(ID id);
    RemoveComponent* createRemoveComponent(ID id);
    AttackComponent* createAttackComponent(ID id);
    CommandComponent* createCommandComponent(ID id);

    void clearComponents(ID id);
};

#endif //COMPONENTMANAGER_H
