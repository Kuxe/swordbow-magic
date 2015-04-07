#ifndef SPATIALINDEXER_H
#define SPATIALINDEXER_H

#include <unordered_set>

using std::unordered_set;

typedef unsigned long long int* ID;
class SpatialIndexer {
	public:

		/**
		 * Simple rect-structure
		 **/
		struct Rect {
			int x, y, w, h;

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
		 *	Clear all cells. Should be followed by an update-method to repopulate the cells.	
		 *	Doesn't remove any ids.
		 **/
		virtual void clear() = 0;

		/**
		 * Fills the list overlappingEntities with all IDs that overlap ID.
		 * Useful for handling collisions.
		 **/
		virtual void overlaps(unordered_set<ID>& overlappingIds, const ID id) = 0;

		/**
		 *	Queries an area for entities, fills the forward_list ids with
		 *	entities within the area. Doesn't guarantees that no objects
		 *	outside the query isnt returned. Useful for retrieving
		 *	entities within an area.
		 **/
		virtual void query(unordered_set<ID>& queryIds, Rect queryArea) = 0;

		
		/**
		 *	Retrieve entities in the same cell as id. Im not sure why this
		 *	is useful when there's query and overlaps.
		 **/
		virtual void getNearbyIds(unordered_set<ID>& nearbyIds, const ID id) = 0;

		/**
		 *	After calling this, all ids in the spatialindexer have their
		 *	positions updated in the spatialindex
		 **/	
		virtual void update() = 0;
};




#endif //SPATIALINDEXER_H
