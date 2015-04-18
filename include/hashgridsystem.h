#ifndef HASHGRIDSYSTEM_H
#define HASHGRIDSYSTEM_H

#include <iostream>
#include "spatialindexer.h"
#include <queue>
#include "isystem.h"

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
		unordered_set<ID>* const cells;
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
		virtual void overlaps(unordered_set<ID>& overlappingIds, const ID id) const;
		virtual void query(unordered_set<ID>& queryIds, const Rect& queryArea) const ;
		virtual void getNearbyIds(unordered_set<ID>& nearbyIds, const ID id) const;

		/**
		 *	Bounding-box test
		 **/
		inline bool intersect(const SpatialIndexer::Rect a, const SpatialIndexer::Rect b) const {
			return !(a.x + a.w < b.x || a.x > b.x + b.w || a.y + a.h < b.y || a.y > b.y + b.h);
		}

		void makeIdActive(const ID id);

		void update();
		unsigned int count() const;
		const string getIdentifier() const;
};




#endif //HASHGRIDSYSTEM_H
