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

using namespace std;

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

	constexpr MoveComponent& createMoveComponent(const ID id) {
        //I must have different ways of creating components here because
        //some components are server-side
        //some components are client-side and
        //some components are both client and server-side
        //Thus componentmanager needs to know what goes where.

        //I only want to initialize the objects once.
        //Therefore the component must be initialized here or before, because
        //it must be initialized before I add it to server-side
        //componentmanagers or client-side componentmanagers.

        //So I could either:

        //1. Pass by value (constructing inside the call to createMoveComponent)
        //2. Pass by reference, (initializing local temps in entitymanager create-methods)

        //I already tried constructing object inside the call to createMoveComponent, but
        //that was a hassle. So next try is to pass already initialized objects
        //by reference. These methods could then be named something like
        //"storeMoveComponent" as opposed to
        //"createMoveComponent"

        //Drawback: entitymanager create methods become larger and more difficult
        //to maintain

        return moveComponents[id] = MoveComponent();
    }
	constexpr RenderComponent& createRenderComponent(const ID id) {    
        return renderComponents[id] = RenderComponent();
    }
	constexpr InputComponent& createInputComponent(const ID id) {
        return inputComponents[id] = InputComponent();
    }
	constexpr TileComponent& createTileComponent(const ID id) {
        return tileComponents[id] = TileComponent();
    }
    constexpr SizeComponent& createSizeComponent(const ID id) {
        return sizeComponents[id] = SizeComponent();
    }
    constexpr NameComponent& createNameComponent(const ID id) {
        return nameComponents[id] = NameComponent();
    }
    constexpr SoundComponent& createSoundComponent(const ID id) {
        return soundComponents[id] = SoundComponent();
    }
    constexpr AnimationComponent& createAnimationComponent(const ID id) {
        return animationComponents[id] = AnimationComponent();
    }
    constexpr HealthComponent& createHealthComponent(const ID id) {
        return healthComponents[id] = HealthComponent();
    }
    constexpr AttackComponent& createAttackComponent(const ID id) {
        return attackComponents[id] = AttackComponent();
    }
    constexpr CommandComponent& createCommandComponent(const ID id) {
        return commandComponents[id] = CommandComponent();
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
            sizeof(CommandComponent);
    }

    void clearComponents(ID id);
};

#endif //COMPONENTMANAGER_H
