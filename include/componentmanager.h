#ifndef COMPONENTMANAGER_H
#define COMPONENTMANAGER_H

#include <unordered_map>

using namespace std;

class MoveComponent;
class RenderComponent;
class InputComponent;
class TileComponent;

class ComponentManager {
 public:
	unordered_map<unsigned long long int *, MoveComponent*> moveComponents;
	unordered_map<unsigned long long int *, RenderComponent*> renderComponents;
	unordered_map<unsigned long long int *, InputComponent*> inputComponents;
	unordered_map<unsigned long long int *, TileComponent*> tileComponents;

	MoveComponent* createMoveComponent(unsigned long long int* id);
	RenderComponent* createRenderComponent(unsigned long long int* id);
	InputComponent* createInputComponent(unsigned long long int* id);
	TileComponent* createTileComponent(unsigned long long int* id);

	void removeAllComponents(unsigned long long int* id);
};

#endif //COMPONENTMANAGER_H