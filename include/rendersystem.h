#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "isystem.h"
#include <unordered_map>
#include <string>
#include <SDL2/SDL.h>

using namespace std;

class RenderComponent;
class MoveComponent;

struct RenderData {
	RenderComponent* renderComponent;
	MoveComponent* moveComponent;
};

struct TextureData {
	SDL_Texture* texture;
	unsigned char width;
	unsigned char height;
};

class RenderSystem : public ISystem {
 private:
	unordered_map<unsigned long long int*, RenderData> renderDatas;
	static constexpr ushort SCREEN_WIDTH = 640;
	static constexpr ushort SCREEN_HEIGHT = 480;

	SDL_Renderer* renderer = nullptr;
	SDL_Window* window = nullptr;
	SDL_Surface* screenSurface = nullptr;
	unordered_map<string, TextureData> textureDatas;
 public:
 	RenderSystem();
 	~RenderSystem();
	void add(unsigned long long int* id);
	void remove(unsigned long long int* id);
	void update();
	unsigned int count() const;
	void render(const RenderData& data) const;
	const string getIdentifier() const;
};

#endif //RENDERSYSTEM_H