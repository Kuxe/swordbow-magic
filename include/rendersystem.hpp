#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "isystem.hpp"
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include "rect.hpp"
#include "text.hpp"
#include "renderdata.hpp"

typedef unsigned int ID;

class Renderer;
class SpatialIndexer;
class CameraSystem;

/** RenderSystem is responsible for taking
	RenderComponents and MoveComponents and
	whatever is required to display the
	game-simulation. Internally it uses
	the Renderer-class to help achieve this
**/
class RenderSystem : public ISystem {
private:
	std::unordered_set<ID> ids;
	std::queue<ID> activeIds;
	std::unordered_map<ID, Rect> oldDrawAreas;

    Renderer* renderer;
    SpatialIndexer* spatialIndexer;
    CameraSystem* cameraSystem;

public:
    RenderSystem(
    	Renderer* renderer,
    	SpatialIndexer* spatialIndexer,
    	CameraSystem* cameraSystem
    );

	void add(ID id);
	void remove(ID id);
	void update();
	unsigned int count() const;
	const System::Identifier getIdentifier() const;
	bool activateId(ID id);
	void printText(const Text& text);
};

#endif //RENDERSYSTEM_H
