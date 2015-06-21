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

class Renderer;

class RenderSystem : public ISystem {
private:
	unordered_set<ID> ids;
	queue<ID> activeIds;
    heap<RenderData> pq;
	queue<Rect> previousDrawAreas;

    Renderer* renderer;

	void renderArea(heap<RenderData>& pq, Rect area);

public:
    RenderSystem(Renderer* renderer);

	void add(ID id);
	void remove(ID id);
	void update();
	unsigned int count() const;
	const string getIdentifier() const;
	void activateId(ID id);
	void setImage(ID id, string path);
	void printText(const Text& text);
    heap<RenderData>& getDrawPriorityQueue();

	//Forces a redraw within an area
	void inline constexpr renderArea(const Rect& area) {
	    previousDrawAreas.push(area);
	}
};

#endif //RENDERSYSTEM_H
