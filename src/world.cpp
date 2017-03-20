#include "world.hpp"
#include <random>
#include "entitymanager.hpp"
#include "noisebrush.hpp"
#include "logger.hpp"
#include "birdsystem.hpp"
#include "systemmanager.hpp"
#include "componentmanager.hpp"

#include <glm/gtc/matrix_transform.hpp> //glm::translate

World::World(EntityManager* entityManager) :
		entityManager(entityManager)
	{ }

void World::createWorld() {

	Logger::log("Creating world...", Logger::INFO);

	//Fill world with grass
	for(short y = 0; y < NUM_TILES; y++) {
		for(short x = 0; x < NUM_TILES; x++) {
			tiles[x][y] = entityManager->createGrassTile(glm::translate(glm::mat4(), {x * TILE_SIZE, 0, y * TILE_SIZE}));
		}
	}

	std::default_random_engine generator;

	//Generate some flower-fields of different colors throughout the map
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
		NoiseBrush noise(randomPoint, fieldRadiusDistribution(generator), 1);

		//For each flowerfield, place some random amounts of flowers
		const int NUM_FLOWERS = flowerNumDistribution(generator);
		for(int j = 0; j < NUM_FLOWERS; j++) {
			//only accept generated flowers within world
			auto point = noise.getPoint();
			if(point.x >= 0 && point.y >= 0) {
				entityManager->createFlower(glm::translate(glm::mat4(), {point.x, 0, point.y}), flowercolor);
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
		NoiseBrush noise(randomPoint, forestRadiusDistribution(generator), 1);

		//For each forest, place some random amounts of trees
		const int NUM_TREES = treeNumDistribution(generator);
		for(int j = 0; j < NUM_TREES; j++) {
			//only accept generated trees within world
			auto point = noise.getPoint();
			if(point.x >= 0 && point.y >= 0) {
				auto id = entityManager->createTree(glm::translate(glm::mat4(), glm::vec3(point.x, 0, point.y)));
			}
		}
	}

	//Place some stones
	std::uniform_real_distribution<double> stoneUniformx(0.0, NUM_TILES * TILE_SIZE);
	std::uniform_real_distribution<double> stoneUniformy(0.0, NUM_TILES * TILE_SIZE);

	const uint16_t NUM_STONES = 100;
	for(uint16_t i = 0; i < NUM_STONES; i++) {
		entityManager->createStone(glm::translate(glm::mat4(), {stoneUniformx(generator), 0, stoneUniformy(generator)}));
	}

	//Create some birds
	auto& componentManager = entityManager->componentManager;
	auto birdSystem = static_cast<BirdSystem*>(entityManager->systemManager->getSystem(System::BIRD));
	std::uniform_real_distribution<double> swarmUniformx(0.0, NUM_TILES * TILE_SIZE);
	std::uniform_real_distribution<double> swarmUniformz(0.0, NUM_TILES * TILE_SIZE);
	constexpr uint16_t birdVariance = 400;
	std::normal_distribution<double> numberOfSwarmsDist(10, 0.5);
	std::normal_distribution<double> numberOfBirdsPerSwarmDist(8, 3);

	const auto numberOfSwarms = numberOfSwarmsDist(generator);
	for(int i = 0; i < numberOfSwarms; i++) {
		const glm::vec3 swarmPos = {swarmUniformx(generator), 0.0f, swarmUniformz(generator)};
		std::normal_distribution<double> birdNormalx(swarmPos.x, birdVariance);
		std::normal_distribution<double> birdNormalz(swarmPos.y, birdVariance);
		const auto swarmId = birdSystem->createSwarm(swarmPos);
		const auto numberOfBirds = numberOfBirdsPerSwarmDist(generator);
		for(int j = 0; j < numberOfBirds; j++) {
			const glm::vec3 birdPos = {birdNormalx(generator), 0.0f, birdNormalz(generator)};
			const auto id = entityManager->createBlueBird(glm::translate(glm::mat4(), birdPos));
			auto& bc = componentManager->birdComponents.at(id);
			bc.swarmId = swarmId;
		}
	}
}

void World::createDebugWorld() {
	//Place tiles in world
	const char NUM_TILES = 8;
	for(short y = 0; y < NUM_TILES; y++) {
		for(short x = 0; x < NUM_TILES; x++) {
			tiles[x][y] = entityManager->createGrassTile(glm::translate(glm::mat4(), glm::vec3(x * TILE_SIZE, 0, y * TILE_SIZE)));
		}
	}

	//This tree might be invisible
	entityManager->createTree({100, 50});

	//This tree (leftmost) probably doesnt get skewed when walking past it
	entityManager->createTree({100, 100});

	//Leaves of this tree probably get skewed if walking behind them
	entityManager->createTree({150.0123f, 127.01f});

	//This flower (leftmost) probably doesnt get skewed when walking past it
	entityManager->createFlower({50, 150}, 0);

	//This flower will probably get skewed if walking past it
	entityManager->createFlower({100.0123f, 177.01f}, 0);

	//test stuff on this guy
	entityManager->createFatMan({50, 100});

	//Create some birds in two separate flocks
	auto& componentManager = entityManager->componentManager;
	auto birdSystem = static_cast<BirdSystem*>(entityManager->systemManager->getSystem(System::BIRD));
	auto swarmId = birdSystem->createSwarm({20, 40, 0});
	std::vector<glm::vec3> birdPoints = {{300, 50, 0}, {20, 30, 0}, {50, 50, 0}, {50, 300, 0}, {62, 86, 0}, {68, 28, 0}};
	for(const auto& point : birdPoints) {
		auto id = entityManager->createBlueBird(point);
		auto& bc = componentManager->birdComponents.at(id);
		bc.swarmId = swarmId;
	}

	swarmId = birdSystem->createSwarm({40, 70, 0});
	birdPoints = {{23, 32, 0}, {123, 15, 0}, {32, 43, 0}, {46, 17, 0}, {57, 32, 0}, {87, 36, 0}};
	for(const auto& point : birdPoints) {
		auto id = entityManager->createBlueBird(point);
		auto& bc = componentManager->birdComponents.at(id);
		bc.swarmId = swarmId;
	}

}
