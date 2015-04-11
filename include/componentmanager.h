#ifndef COMPONENTMANAGER_H
#define COMPONENTMANAGER_H

#include <unordered_map>

using namespace std;

class MoveComponent;
class RenderComponent;
class InputComponent;
class TileComponent;
class FlagComponent;
class SizeComponent;
class NameComponent;

class ComponentManager {
 public:
	unordered_map<unsigned long long int *, MoveComponent*> moveComponents;
	unordered_map<unsigned long long int *, RenderComponent*> renderComponents;
	unordered_map<unsigned long long int *, InputComponent*> inputComponents;
	unordered_map<unsigned long long int *, TileComponent*> tileComponents;
	unordered_map<unsigned long long int *, FlagComponent*> flagComponents;
	unordered_map<unsigned long long int *, SizeComponent*> sizeComponents;
    unordered_map<unsigned long long int *, NameComponent*> nameComponents;

	MoveComponent* createMoveComponent(unsigned long long int* id);
	RenderComponent* createRenderComponent(unsigned long long int* id);
	InputComponent* createInputComponent(unsigned long long int* id);
	TileComponent* createTileComponent(unsigned long long int* id);
	FlagComponent* createFlagComponent(unsigned long long int* id);
	SizeComponent* createSizeComponent(unsigned long long int* id);
    NameComponent* createNameComponent(unsigned long long int* id);

	void removeAllComponents(unsigned long long int* id);
};

#endif //COMPONENTMANAGER_H
