#include "world.hpp"
#include "entitymanager.hpp"
#include "componentmanager.hpp"
#include "movecomponent.hpp"
#include "rendercomponent.hpp"
#include "sizecomponent.hpp"
#include <iostream>
#include <glm/ext.hpp>

using namespace std;

World::World(EntityManager* entityManager) :
		entityManager(entityManager)
	{

	//Place tiles in world
	for(short y = 0; y < NUM_TILES; y++) {
		for(short x = 0; x < NUM_TILES; x++) {
			auto id  = entityManager->createTile();
			tiles[x][y] = id;

			//Align tiles into a grid, spatially
			auto& mc = entityManager->componentManager->moveComponents.at(id);
			mc.pos.x = x * TILE_SIZE;
			mc.pos.y = y * TILE_SIZE;
			mc.oldPos.x = x * TILE_SIZE;
			mc.oldPos.y = y * TILE_SIZE;

			auto sc = entityManager->componentManager->sizeComponents.at(id);
			sc.width = TILE_SIZE;
			sc.height = TILE_SIZE;
		}
	}


	//Generate some flower-fields of different colors throughout the map
	std::default_random_engine generator;
	std::uniform_real_distribution<double> uniformx(0.0, NUM_TILES * TILE_SIZE);
	std::uniform_real_distribution<double> uniformy(0.0, NUM_TILES * TILE_SIZE);
	std::normal_distribution<double> flowerNumDistribution(100.0, 50.0);
	std::normal_distribution<double> fieldRadiusDistribution(50, 30);
	std::uniform_real_distribution<double> flowercolorDistribution(0.0, 3.9999);


	const int NUM_FLOWERFIELDS = 150;
	for(int i = 0; i < NUM_FLOWERFIELDS; i++) {

		//Chose what type of flower to plant
		char flowercolor = flowercolorDistribution(generator);

		//Create flowerfields by creating some noisebrushes uniformly
		//throughout the map
		const glm::vec2 randomPoint(uniformx(generator), uniformy(generator));
		NoiseBrush noise(randomPoint, fieldRadiusDistribution(generator), 1, SEED);

		//For each flowerfield, place some random amounts of flowers
		const int NUM_FLOWERS = flowerNumDistribution(generator);
		for(int j = 0; j < NUM_FLOWERS; j++) {
			//only accept generated flowers within world
			auto point = noise.getPoint();
			if(point.x >= 0 && point.y >= 0) {
				auto id = entityManager->createFlower(point, flowercolor);
			}
		}
	}

	//Generate some forests throughout the map
	std::normal_distribution<double> treeNumDistribution(200, 100);
	std::normal_distribution<double> forestRadiusDistribution(500, 250);

	const int NUM_FORESTS = 30;
	for(int i = 0; i < NUM_FORESTS; i++) {

		//Create forests by creating some noisebrushes uniformly
		//throughout the map
		const glm::vec2 randomPoint(uniformx(generator), uniformy(generator));
		NoiseBrush noise(randomPoint, forestRadiusDistribution(generator), 1, SEED);

		//For each forest, place some random amounts of trees
		const int NUM_TREES = treeNumDistribution(generator);
		for(int j = 0; j < NUM_TREES; j++) {
			//only accept generated trees within world
			auto point = noise.getPoint();
			if(point.x >= 0 && point.y >= 0) {
				auto id = entityManager->createTree(point);
			}
		}
	}
}
