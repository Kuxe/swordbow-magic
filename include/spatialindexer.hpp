#ifndef SPATIALINDEXER_H
#define SPATIALINDEXER_H

#include <unordered_set>

#include "rect.hpp"

typedef unsigned int ID;
class SpatialIndexer {
	public:

		virtual ~SpatialIndexer() { }

		/**
		 *	Add an ID to the spatialindexer. This ID can now be returned by any retrieval-method
		 **/
		virtual void add(ID id) = 0;

		/**
		 *	Removes an ID from the spatialindexer. This ID wont be returned by any retrieval-method
		 **/
		virtual void remove(ID id) = 0;

		/**
		 * Fills the list overlappingEntities with all IDs that overlap IDs texture.
		 * Useful for overlapping textures.
		 **/
		virtual const std::unordered_set<ID> overlaps(const ID id) const = 0;

		/**
		 *	Return all ids within queryarea
		 **/
		virtual const std::unordered_set<ID> query(const Rect& queryArea) const = 0;

		virtual Rect getBoundingBox(const ID id) const = 0;

		/**
		 *	After calling this, all ids in the spatialindexer have their
		 *	positions updated in the spatialindex
		 **/
		virtual void update() = 0;
};



#endif //SPATIALINDEXER_H
