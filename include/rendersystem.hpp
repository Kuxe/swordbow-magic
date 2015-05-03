#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "isystem.hpp"
#include <unordered_map>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include <unordered_set>
#include <queue>
#include "spatialindexer.hpp"
#include "heap.hpp"
#include "text.hpp"

using namespace std;

class RenderComponent;
class MoveComponent;
class SizeComponent;
class FlagComponent;
class SpatialIndexer;

struct TextureData {
	SDL_Texture* texture;
	unsigned int width;
	unsigned int height;
};

struct RenderData {
	unsigned long long int* id;
	RenderComponent* renderComponent;
	TextureData textureData;
	SDL_Rect cliprect;
	SDL_Rect target;

	inline bool operator< (const RenderData& rhs) const;
    inline bool operator> (const RenderData& rhs) const;
    inline bool operator<=(const RenderData& rhs) const;
    inline bool operator>=(const RenderData& rhs) const;
};

class RenderSystem : public ISystem {
 private:
	unordered_set<unsigned long long int*> ids;
	queue<unsigned long long int*> activeIds;
	queue<SpatialIndexer::Rect> previousDrawAreas;
	queue<Text> texts;


	static constexpr ushort SCREEN_WIDTH = 640;
	static constexpr ushort SCREEN_HEIGHT = 480;
	bool renderingRequired = true;

	SDL_Renderer* renderer = nullptr;
	SDL_Window* window = nullptr;
	SDL_Texture* worldTexture = nullptr;
	SDL_Texture* fontTexture = nullptr;
	TTF_Font* font;
	unordered_map<string, TextureData> textureDatas;

	unsigned long long int* cameraTarget;

	void renderArea(heap<RenderData>& pq, SpatialIndexer::Rect area);
	void renderTexts();

 public:
 	RenderSystem();
 	~RenderSystem();
	void add(unsigned long long int* id);
	void remove(unsigned long long int* id);
	void update();
	void update2();
	unsigned int count() const;
	void render(const RenderData& rd) const;
	const string getIdentifier() const;
	void calculateZIndex(unsigned long long int* id);
	void activateId(unsigned long long int* id);
	void setCameraTarget(unsigned long long int* id);
	void setImage(unsigned long long int* id, string path);
	void printText(const Text& text);
};

#endif //RENDERSYSTEM_H
