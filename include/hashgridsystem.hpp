#ifndef HASHGRIDSYSTEM_H
#define HASHGRIDSYSTEM_H

#include <queue>
#include "spatialindexer.hpp"
#include "isystem.hpp"

class BoundingBox;

class HashGridSystem : public SpatialIndexer, public ISystem {
	private:
		std::unordered_set<ID> ids;
		std::queue<ID> activeIds;
		const unsigned int width;
		const unsigned int height;
		const unsigned int cellsCapacity;
		std::unordered_set<ID>* cells;
		const unsigned int side;

		BoundingBox* boundingBox;

		void addToCells(const ID id);
		void removeFromCells(const ID id);
		void removeFromCellsOldBoundingBox(const ID id);

	public:

		HashGridSystem(
			BoundingBox* boundingBox,
			const unsigned int worldWidth = 32768,
			const unsigned int worldHeight = 32786,
			const unsigned int side = 64);

		virtual ~HashGridSystem();

		virtual void add(ID id);
		virtual void remove(ID id);
		virtual std::unordered_set<ID> overlaps(const ID id) const;
		virtual std::unordered_set<ID> query(const Rect& queryArea) const ;
		inline Rect getBoundingBox(const ID id) const;

		bool activateId(ID id);

		void update();
		unsigned int count() const;
		const System::Identifier getIdentifier() const;
};




#endif //HASHGRIDSYSTEM_H
