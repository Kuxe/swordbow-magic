#ifndef SPATIALINDEXER_H
#define SPATIALINDEXER_H

#include <unordered_set>

using std::unordered_set;

typedef unsigned int ID;
class SpatialIndexer {
	public:

		virtual ~SpatialIndexer() { }

		/**
		 * Simple rect-structure
		 **/
		struct Rect {
			unsigned int x, y, w, h;

			inline bool operator==(const Rect& rhs) const {
				return x == rhs.x && y == rhs.y && w == rhs.w && h == rhs.h;
			}
			inline bool operator!=(const Rect& rhs) const {
				return !(*this == rhs);
			}
		};

		/**
		 *	An entity is spatially represented as a rect with an ID
		 **/
		struct Entity {
			Rect rect;
			ID id;

			inline bool operator==(const Entity& rhs) const {
				return rect == rhs.rect && id == rhs.id;
			}
			inline bool operator!=(const Entity& rhs) const {
				return !(*this == rhs);
			}
		};

		/**
		 *	A collision consists of two ID's that collided
		**/
		struct Overlap {
			ID a;
			ID b;

			inline bool operator==(const Overlap& rhs) const {
				return a == rhs.a && b == rhs.b;
			}

			inline bool operator!=(const Overlap& rhs) const {
				return !(*this == rhs);
			}
		};

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
		virtual void overlaps(unordered_set<ID>& overlappingIds, const ID id) const = 0;

		/**
		 *	Return all ids within queryarea
		 **/
		virtual void query(unordered_set<ID>& queryIds, const Rect& queryArea) const = 0;


		/**
		 *	Retrieve entities in the same cell as id. Im not sure why this
		 *	is useful when there's query and overlaps.
		 **/
		virtual void getNearbyIds(unordered_set<ID>& nearbyIds, const ID id) const = 0;

		virtual void getIntersectionArea(Rect& intersectionArea,	const Rect& a, const Rect& b) const = 0;

		virtual void getBoundingBox(Rect& boundingBox, const ID id) const = 0;

		/**
		 *	After calling this, all ids in the spatialindexer have their
		 *	positions updated in the spatialindex
		 **/
		virtual void update() = 0;
};




#endif //SPATIALINDEXER_H
