#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "isystem.h"
#include <unordered_map>

using namespace std;

class SDL_Window;
class SDL_Surface;

class RenderComponent;
class MoveComponent;

struct RenderData {
	RenderComponent* renderComponent;
	MoveComponent* moveComponent;
};

class RenderSystem : public ISystem {
 private:
	unordered_map<unsigned long long int*, RenderData> renderDatas;

	const ushort SCREEN_WIDTH = 640;
	const ushort SCREEN_HEIGHT = 480;
	SDL_Window* window = nullptr;
	SDL_Surface* screenSurface = nullptr;

 public:
 	RenderSystem();
 	~RenderSystem();
	void add(unsigned long long int* id);
	void remove(unsigned long long int* id);
	void update();
	unsigned int count() const;
	void render(RenderData data);
	const string getIdentifier() const;
};

#endif //RENDERSYSTEM_H