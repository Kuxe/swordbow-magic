#ifndef WORLD_H
#define WORLD_H

class EntityManager;
class World {
 private:
	static constexpr short NUM_TILES = 128;
	static constexpr unsigned char TILE_SIZE = 20;
	unsigned long long int* tiles[NUM_TILES][NUM_TILES];

	EntityManager* entityManager;
 public:
	explicit World(EntityManager* entityManager);

};

#endif //WORLD_H
