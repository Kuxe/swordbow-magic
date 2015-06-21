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
#include "renderdata.hpp"
#include "texturedata.hpp"

using namespace std;

class RenderComponent;
class MoveComponent;
class SizeComponent;
class FlagComponent;
class SpatialIndexer;

typedef unsigned int ID;

class CameraSystem;

class RenderSystem : public ISystem {
 private:
	unordered_set<ID> ids;
	queue<ID> activeIds;
	queue<Rect> previousDrawAreas;
	queue<Text> texts;
	CameraSystem* cameraSystem;


	static constexpr ushort SCREEN_WIDTH = 1920 / 4;
	static constexpr ushort SCREEN_HEIGHT = 1080 / 4;

	SDL_Renderer* renderer = nullptr;
	SDL_Window* window = nullptr;
	SDL_Texture* worldTexture = nullptr;
	SDL_Texture* fontTexture = nullptr;
	TTF_Font* font;
	unordered_map<string, TextureData> textureDatas;

	void renderArea(heap<RenderData>& pq, Rect area);
	void renderTexts();

 public:
 	RenderSystem();
 	~RenderSystem();
	void add(ID id);
	void remove(ID id);
	void update();
	unsigned int count() const;
	void render(const RenderData& rd) const;
	const string getIdentifier() const;
	void activateId(ID id);
	void setCameraSystem(CameraSystem* cameraSystem);
	void setImage(ID id, string path);
	void printText(const Text& text);
    const unordered_map<string, TextureData>& getTextureDatas() const;

	//Forces a redraw within an area
	void inline constexpr renderArea(const Rect& area) {
	    previousDrawAreas.push(area);
	}
	static constexpr ushort getScreenWidth() { return SCREEN_WIDTH; }
    static constexpr ushort getScreenHeight() { return SCREEN_HEIGHT; }

};

#endif //RENDERSYSTEM_H
