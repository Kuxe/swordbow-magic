#include "componentmanager.h"
#include "movecomponent.h"
#include "rendercomponent.h"
#include "inputcomponent.h"
#include <iostream>

using namespace std;

MoveComponent* ComponentManager::createMoveComponent(unsigned long long int* id) {
	MoveComponent* moveComponent = new MoveComponent();
	moveComponents.insert(make_pair(id, moveComponent));
	return moveComponent;
}
RenderComponent* ComponentManager::createRenderComponent(unsigned long long int* id) {
	RenderComponent* renderComponent = new RenderComponent();
	renderComponents.insert(make_pair(id, renderComponent));
	return renderComponent;
}

InputComponent* ComponentManager::createInputComponent(unsigned long long int* id) {
	InputComponent* inputComponent = new InputComponent();
	inputComponents.insert(make_pair(id, inputComponent));
	return inputComponent;
}

void ComponentManager::removeAllComponents(unsigned long long int* id) {
	try {
		delete moveComponents.at(id);
	} catch (const std::out_of_range& oor) {
		cout << "id has no MoveComponent" << endl;
	}

	try {
		delete renderComponents.at(id);
	} catch (const std::out_of_range& oor) {
		cout << "id has no renderComponents" << endl;
	}

	try {
		delete inputComponents.at(id);
	} catch (const std::out_of_range& oor) {
		cout << "id has no renderComponents" << endl;
	}
}