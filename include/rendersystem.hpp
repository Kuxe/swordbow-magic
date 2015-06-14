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
#include "glm/vec2.hpp"

using namespace std;

class RenderComponent;
class MoveComponent;
class SizeComponent;
class FlagComponent;
class SpatialIndexer;

typedef unsigned int ID;

class CameraSystem;

struct TextureData {
	SDL_Texture* texture;
	unsigned int width;
	unsigned int height;
};

struct RenderData {
	ID id;
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
	void calculateZIndex(ID id);
	void activateId(ID id);
	void setCameraSystem(CameraSystem* cameraSystem);
	void setImage(ID id, string path);
	void printText(const Text& text);

	//Converts a intersection (rectangle in space)
	//to a clip source (where to clip an image for rendering)
	//giving the clip rect for the image within the intersection
	static inline SDL_Rect intersectionToClipSource(
	    	const Rect& intersection,
	    	const glm::vec2& pos,
	    	const float xoffset, const float yoffset) {

		int x = intersection.x - pos.x - xoffset;
		int y = intersection.y - pos.y - yoffset;

		float xdiff = intersection.x - pos.x - xoffset - (float)x;
		float ydiff = intersection.y - pos.y - yoffset - (float)y;

		int w = (intersection.w + xdiff);
		int h = (intersection.h + ydiff);
		return {x, y, w, h};
	}

	//Converts a intersection (rectangle in space)
	//to a clip source (where to clip an image for rendering)
	//giving the clip rect for the image within the intersection
	static inline SDL_Rect intersectionToClipDestination(
	    const Rect& intersection) {

			int x = intersection.x;
			int y = intersection.y;

			float xdiff = intersection.x - (float)x;
			float ydiff = intersection.y - (float)y;

			int w = (intersection.w + xdiff);
			int h = (intersection.h + ydiff);

	        return {x, y, w, h};
	}

	//Forces a redraw within an area
	void inline constexpr renderArea(const Rect& area) {
	    previousDrawAreas.push(area);
	}
	static constexpr ushort getScreenWidth() { return SCREEN_WIDTH; }
    static constexpr ushort getScreenHeight() { return SCREEN_HEIGHT; }

};

#endif //RENDERSYSTEM_H
