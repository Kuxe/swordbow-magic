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
	std::queue<ID> activeIds;
    std::priority_queue<RenderData> pq;
	std::queue<Rect> previousDrawAreas;

    Renderer* renderer;

	void renderArea(heap<RenderData>& pq, Rect area);

public:
    RenderSystem(Renderer* renderer);

	void add(ID id);
	void remove(ID id);
	void update();
	unsigned int count() const;
	const System::Identifier getIdentifier() const;
	void activateId(ID id);
	void printText(const Text& text);

	//TODO: Implement const ref
    std::priority_queue<RenderData>& getDrawPriorityQueue();
	const std::priority_queue<RenderData>& getDrawPriorityQueue() const;

	//Forces a redraw within an area
	void inline constexpr renderArea(const Rect& area) {
	    previousDrawAreas.push(area);
	}
};

#endif //RENDERSYSTEM_H
