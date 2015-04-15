#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "isystem.h"
#include <unordered_map>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>
#include <unordered_set>
#include <queue>

using namespace std;

class RenderComponent;
class MoveComponent;
class SizeComponent;
class FlagComponent;
class SpatialIndexer;

struct RenderData {
	RenderComponent* renderComponent;
	MoveComponent* moveComponent;
	SizeComponent* sizeComponent;
	FlagComponent* flagComponent;

	inline bool operator< (const RenderData& rhs) const;
	inline bool operator> (const RenderData& rhs) const;
	inline bool operator<=(const RenderData& rhs) const;
	inline bool operator>=(const RenderData& rhs) const;
};

struct TextureData {
	SDL_Texture* texture;
	unsigned int width;
	unsigned int height;
};

class RenderSystem : public ISystem {
 private:
 	static constexpr unsigned char MAX_IDS = 255;
 	//unsigned long long int* ids[MAX_IDS] {0};
	unordered_map<unsigned long long int*, RenderData> renderDatas;
	unordered_set<unsigned long long int*> ids;
	queue<unsigned long long int*> activeIds;


	static constexpr ushort SCREEN_WIDTH = 640;
	static constexpr ushort SCREEN_HEIGHT = 480;
	bool renderingRequired = true;

	SDL_Renderer* renderer = nullptr;
	SDL_Window* window = nullptr;
	SDL_Surface* screenSurface = nullptr;
	SDL_Texture* targetTexture = nullptr;
	unordered_map<string, TextureData> textureDatas;
	SpatialIndexer* spatialIndexer;
 public:
 	RenderSystem(SpatialIndexer* spatialIndexer);
 	~RenderSystem();
	void add(unsigned long long int* id);
	void remove(unsigned long long int* id);
	void update();
	void update2();
	unsigned int count() const;
	void render(const RenderData& data) const;
	const string getIdentifier() const;
	void calculateZIndex(unsigned long long int* id);
	void makeIdActive(unsigned long long int* id);
};

#endif //RENDERSYSTEM_H
