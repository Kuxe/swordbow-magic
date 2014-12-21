#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "isystem.h"
#include <unordered_map>

using namespace std;

class ComponentManager;
class RenderComponent;
class MoveComponent;

struct RenderData {
	RenderComponent* renderComponent;
	MoveComponent* moveComponent;
};

class RenderSystem : public ISystem {
 private:
	unordered_map<unsigned long long int*, RenderData> renderDatas;

 public:
	void add(unsigned long long int* id, RenderData data);
	void remove(unsigned long long int* id);
	void update();
	unsigned int count() const;
	void render(RenderData data);
	const string getIdentifier() const;
};

#endif //RENDERSYSTEM_H