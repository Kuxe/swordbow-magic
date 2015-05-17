#ifndef HASHGRIDSYSTEM_H
#define HASHGRIDSYSTEM_H

#include <iostream>
#include "spatialindexer.hpp"
#include <queue>
#include "isystem.hpp"

using std::cout;
using std::endl;
using std::queue;

class ComponentManager;
class BoundingBox;

class HashGridSystem : public SpatialIndexer, public ISystem {
	private:
		unordered_set<ID> ids;
		queue<ID> activeIds;
		const unsigned int cellsCapacity;
		const unsigned int width;
		const unsigned int height;
		unordered_set<ID>* cells;
		const unsigned int side;

		ComponentManager* componentManager;

		BoundingBox* boundingBox;

		void addToCells(const ID id);
		void removeFromCells(const ID id);
		void removeFromCellsOldBoundingBox(const ID id);

	public:

		HashGridSystem(
			ComponentManager* componentManager,
			BoundingBox* boundingBox,
			const unsigned int worldWidth = 16384,
			const unsigned int worldHeight = 16384,
			const unsigned int side = 64);

		virtual ~HashGridSystem();

		virtual void add(ID id);
		virtual void remove(ID id);
		virtual unordered_set<ID> overlaps(const ID id) const;
		virtual unordered_set<ID> query(const Rect& queryArea) const ;
		virtual unordered_set<ID> getNearbyIds(const ID id) const;

		/**
		 *	Bounding-box test
		 **/
		inline bool intersect(const SpatialIndexer::Rect& a, const SpatialIndexer::Rect& b) const {
			return !(a.x + a.w < b.x || a.x > b.x + b.w || a.y + a.h < b.y || a.y > b.y + b.h);
		}

		inline SpatialIndexer::Rect getIntersectionArea(
			const SpatialIndexer::Rect& a,
			const SpatialIndexer::Rect& b) const {

			//TODO: This can probably be optimized
			const unsigned int x = a.x >= b.x ? a.x : b.x;
			const unsigned int y = a.y >= b.y ? a.y : b.y;
			const unsigned int w = (a.x + a.w) <= (b.x + b.w) ? a.x + a.w - x : b.x + b.w - x;
			const unsigned int h = (a.y + a.h) <= (b.y + b.h) ? a.y + a.h - y : b.y + b.h - y;
			return {x, y, w, h};
		}

		inline SpatialIndexer::Rect getBoundingBox(const ID id) const;

		void activateId(ID id);

		void update();
		unsigned int count() const;
		const string getIdentifier() const;
};




#endif //HASHGRIDSYSTEM_H
