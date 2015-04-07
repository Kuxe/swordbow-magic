#ifndef HASHGRID_H
#define HASHGRID_H

#include "spatialindexer.h"

class ComponentManager;

class HashGrid : public SpatialIndexer {
	private:
		unordered_set<ID> ids;
		unordered_set<ID>* cells;
		unsigned int cellsCapacity;
		const unsigned int width;
		const unsigned int height;
		const unsigned int side;

		ComponentManager* componentManager;

		void addToCells(const ID id);

	public:

		HashGrid(ComponentManager* componentManager, unsigned int width = 250, unsigned int height = 250, unsigned int side = 50);
		virtual ~HashGrid();
		
		virtual void add(const ID id);
		virtual void remove(const ID id);
		virtual void clear();
		virtual void overlaps(unordered_set<ID>& overlappingIds, const ID id);
		virtual void query(unordered_set<ID>& queryIds, Rect queryArea);
		virtual void getNearbyIds(unordered_set<ID>& nearbyIds, const ID id);

		/**
		 *	Bounding-box test
		 **/
		inline bool intersect(const SpatialIndexer::Rect a, const SpatialIndexer::Rect b) const {
			return a.x + a.w <= b.x && a.x >= b.x + b.w && a.y + a.h <= b.y && a.y >= b.y + b.h;	
		}

		void update();
};




#endif //HASHGRID_H
