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

	constexpr MoveComponent* createMoveComponent(ID id) {
        return &(moveComponents[id] = MoveComponent());
    }
	constexpr RenderComponent* createRenderComponent(ID id) {
        return &(renderComponents[id] = RenderComponent());
    }
	constexpr InputComponent* createInputComponent(ID id) {
        return &(inputComponents[id] = InputComponent());
    }
	constexpr TileComponent* createTileComponent(ID id) {
        return &(tileComponents[id] = TileComponent());
    }
    constexpr SizeComponent* createSizeComponent(ID id) {
        return &(sizeComponents[id] = SizeComponent());
    }
    constexpr NameComponent* createNameComponent(ID id) {
        return &(nameComponents[id] = NameComponent());
    }
    constexpr MoveEventComponent* createMoveEventComponent(ID id) {
        return &(moveEventComponents[id] = MoveEventComponent());
    }
    constexpr ReleaseKeyEventComponent* createReleaseKeyEventComponent(ID id) {
        return &(releaseKeyEventComponents[id] = ReleaseKeyEventComponent());
    }
    constexpr SoundComponent* createSoundComponent(ID id) {
        return &(soundComponents[id] = SoundComponent());
    }
    constexpr AnimationComponent* createAnimationComponent(ID id) {
        return &(animationComponents[id] = AnimationComponent());
    }
    constexpr HealthComponent* createHealthComponent(ID id) {
        return &(healthComponents[id] = HealthComponent());
    }
    constexpr RemoveComponent* createRemoveComponent(ID id) {
        return &(removeComponents[id] = RemoveComponent());
    }
    constexpr AttackComponent* createAttackComponent(ID id) {
        return &(attackComponents[id] = AttackComponent());
    }
    constexpr CommandComponent* createCommandComponent(ID id) {
        return &(commandComponents[id] = CommandComponent());
    }

    void clearComponents(ID id);
};

#endif //COMPONENTMANAGER_H
