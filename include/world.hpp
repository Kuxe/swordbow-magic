#ifndef WORLD_H
#define WORLD_H

typedef unsigned int ID;

class EntityManager;
class World {
 private:
	static constexpr short NUM_TILES = 16;
	static constexpr unsigned char TILE_SIZE = 20;
	ID tiles[NUM_TILES][NUM_TILES];

	EntityManager* entityManager;
 public:
	explicit World(EntityManager* entityManager);

};

#endif //WORLD_H
