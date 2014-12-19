/* Defines
	
	DEBUG: enable debug-logging to terminal

*/
#define DEBUG
#include <iostream>
#include "componentmanager.h"
#include "systemmanager.h"
#include "idmanager.h"
#include "entitymanager.h"
#include "movesystem.h"
#include "rendersystem.h"
#include "movecomponent.h"
#include "rendercomponent.h"
#include "player.h"

using namespace std;

int main(int argc, char** argv) {

	bool running = true;

	SystemManager systemManager;
	ComponentManager componentManager;
	IdManager idManager;

	EntityManager entityManager(&systemManager, &componentManager, &idManager);

	MoveSystem moveSystem;
	RenderSystem renderSystem;

	systemManager.add(&moveSystem);
	systemManager.add(&renderSystem);

	/* TESTING: Make sure a player can be created, updated, removed without breaking the systems */
	unsigned long long int* playerId = entityManager.createPlayer();
	for(int i = 0; i < 20; i++) {
		systemManager.update();
	}
	entityManager.removePlayer(playerId);
	for(int i = 0; i < 20; i++) {
		systemManager.update();
	}
	/* End of TESTING */

	return 0;
}