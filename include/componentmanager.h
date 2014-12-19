#ifndef COMPONENTMANAGER_H
#define COMPONENTMANAGER_H

#include <unordered_map>

using namespace std;

class MoveComponent;
class RenderComponent;

class ComponentManager {
 public:
	unordered_map<unsigned long long int *, MoveComponent*> moveComponents;
	unordered_map<unsigned long long int *, RenderComponent*> renderComponents;

	MoveComponent* createMoveComponent(unsigned long long int* id);
	RenderComponent* createRenderComponent(unsigned long long int* id);

	void removeAllComponents(unsigned long long int* id);
};

#endif //COMPONENTMANAGER_H