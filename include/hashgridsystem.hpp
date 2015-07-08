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
		const unsigned int width;
		const unsigned int height;
		const unsigned int cellsCapacity;
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
			const unsigned int worldWidth = 32768,
			const unsigned int worldHeight = 32786,
			const unsigned int side = 64);

		virtual ~HashGridSystem();

		virtual void add(ID id);
		virtual void remove(ID id);
		virtual unordered_set<ID> overlaps(const ID id) const;
		virtual unordered_set<ID> query(const Rect& queryArea) const ;
		inline Rect getBoundingBox(const ID id) const;

		void activateId(ID id);

		void update();
		unsigned int count() const;
		const System getIdentifier() const;
};




#endif //HASHGRIDSYSTEM_H
