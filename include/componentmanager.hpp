#ifndef COMPONENTMANAGER_H
#define COMPONENTMANAGER_H

#include "components.hpp"
#include "animationcomponent.hpp"
#include "movecomponent.hpp"
#include "rendercomponent.hpp"
#include "inputcomponent.hpp"
#include "tilecomponent.hpp"
#include "sizecomponent.hpp"
#include "namecomponent.hpp"
#include "soundcomponent.hpp"
#include "healthcomponent.hpp"
#include "attackcomponent.hpp"
#include "commandcomponent.hpp"
#include "accelerationcomponent.hpp"
#include "birdcomponent.hpp"

typedef unsigned int ID;

class ComponentManager {
 public:
	Components<MoveComponent> moveComponents;
	Components<RenderComponent> renderComponents;
	Components<InputComponent> inputComponents;
	Components<TileComponent> tileComponents;
	Components<SizeComponent> sizeComponents;
    Components<NameComponent> nameComponents;
    Components<SoundComponent> soundComponents;
    Components<AnimationComponent> animationComponents;
    Components<HealthComponent> healthComponents;
    Components<AttackComponent> attackComponents;
    Components<CommandComponent> commandComponents;
    Components<AccelerationComponent> accelerationComponents;
    Components<BirdComponent> birdComponents;

    //Components<MoveComponent> moveComponentsDiff;
    //Components<RenderComponent> renderComponentsDiff;

	MoveComponent& createMoveComponent(const ID id) {
        return moveComponents[id] = MoveComponent();
    }
	RenderComponent& createRenderComponent(const ID id) {
        return renderComponents[id] = RenderComponent();
    }
	InputComponent& createInputComponent(const ID id) {
        return inputComponents[id] = InputComponent();
    }
	TileComponent& createTileComponent(const ID id) {
        return tileComponents[id] = TileComponent();
    }
    SizeComponent& createSizeComponent(const ID id) {
        return sizeComponents[id] = SizeComponent();
    }
    NameComponent& createNameComponent(const ID id) {
        return nameComponents[id] = NameComponent();
    }
    SoundComponent& createSoundComponent(const ID id) {
        return soundComponents[id] = SoundComponent();
    }
    AnimationComponent& createAnimationComponent(const ID id) {
        return animationComponents[id] = AnimationComponent();
    }
    HealthComponent& createHealthComponent(const ID id) {
        return healthComponents[id] = HealthComponent();
    }
    AttackComponent& createAttackComponent(const ID id) {
        return attackComponents[id] = AttackComponent();
    }
    CommandComponent& createCommandComponent(const ID id) {
        return commandComponents[id] = CommandComponent();
    }
    AccelerationComponent& createAccelerationComponent(const ID id) {
        return accelerationComponents[id] = AccelerationComponent();
    }
    BirdComponent& createBirdComponent(const ID id) {
        return birdComponents[id] = BirdComponent();
    }

    static constexpr uint sizePerEntity() {
        return
            sizeof(MoveComponent) +
            sizeof(RenderComponent) +
            sizeof(InputComponent) +
            sizeof(TileComponent) +
            sizeof(SizeComponent) +
            sizeof(NameComponent) +
            sizeof(SoundComponent) +
            sizeof(AnimationComponent) +
            sizeof(HealthComponent) +
            sizeof(AttackComponent) +
            sizeof(CommandComponent) +
            sizeof(AccelerationComponent);
    }

    void clearComponents(ID id);
};

#endif //COMPONENTMANAGER_H
