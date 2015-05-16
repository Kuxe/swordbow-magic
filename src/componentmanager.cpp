#include "componentmanager.hpp"

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

ReleaseKeyEventComponent* ComponentManager::createReleaseKeyEventComponent(ID id) {
	releaseKeyEventComponents.insert(make_pair(id, ReleaseKeyEventComponent()));
	return &releaseKeyEventComponents.at(id);
}

SoundComponent* ComponentManager::createSoundComponent(ID id) {
	soundComponents.insert(make_pair(id, SoundComponent()));
	return &soundComponents.at(id);
}

AnimationComponent* ComponentManager::createAnimationComponent(ID id) {
	animationComponents.insert(make_pair(id, AnimationComponent()));
	return &animationComponents.at(id);
}

HealthComponent* ComponentManager::createHealthComponent(ID id) {
	healthComponents.insert(make_pair(id, HealthComponent()));
	return &healthComponents.at(id);
}

RemoveComponent* ComponentManager::createRemoveComponent(ID id) {
	removeComponents.insert(make_pair(id, RemoveComponent()));
	return &removeComponents.at(id);
}

AttackComponent* ComponentManager::createAttackComponent(ID id) {
	attackComponents.insert(make_pair(id, AttackComponent()));
	return &attackComponents.at(id);
}

CommandComponent* ComponentManager::createCommandComponent(ID id) {
	commandComponents.insert(make_pair(id, CommandComponent()));
	return &commandComponents.at(id);
}

void ComponentManager::clearComponents(ID id) {
	moveComponents.erase(id);
	renderComponents.erase(id);
	inputComponents.erase(id);
	tileComponents.erase(id);
	sizeComponents.erase(id);
    nameComponents.erase(id);
    releaseKeyEventComponents.erase(id);
    moveEventComponents.erase(id);
    soundComponents.erase(id);
    animationComponents.erase(id);
    healthComponents.erase(id);
    removeComponents.erase(id);
    attackComponents.erase(id);
    commandComponents.erase(id);
};
