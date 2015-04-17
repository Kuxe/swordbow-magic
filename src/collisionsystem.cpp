#include "collisionsystem.h"
#include "componentmanager.h"
#include "movecomponent.h"
#include "sizecomponent.h"

constexpr bool CollisionSystem::colliding(const CollisionData& a, const CollisionData& b) {

	/**
	 * Two boxed bounded by
	 * {1, 2, 3, 4} and
	 * {5, 6, 7, 8}
	 *	1-----2
	 *  |     |
	 *  |  5--|--6
	 *  |  |  |  |
	 *  3-----4  |
	 *     |     |
	 *     7-----8
	 *
	 **/

	//Check if any border doesn't overlap
	return !(a.mc->xpos + a.sc->width < b.mc->xpos ||
			a.mc->xpos > b.mc->xpos + b.sc->width ||
			a.mc->ypos + a.sc->height < b.mc->ypos ||
			a.mc->ypos > b.mc->ypos + b.sc->height );
}

void CollisionSystem::add(unsigned long long int* id) {
	collisionDatas.add({
			componentManager->moveComponents.at(id),
			componentManager->sizeComponents.at(id)
		}
	);
}

void CollisionSystem::remove(unsigned long long int* id) {
	//O(n) due to dynamicarray. Should be hashset instead.
	collisionDatas.remove({
				componentManager->moveComponents.at(id),
				componentManager->sizeComponents.at(id),
	});
}

void CollisionSystem::update() {
	//Naïve O(n^2) implementation, compare everything with everything
	for(unsigned int outer = 0; outer < collisionDatas.getSize(); outer++) {
		for(unsigned int inner = outer+1; inner < collisionDatas.getSize(); inner++) {
			if(colliding(collisionDatas[outer], collisionDatas[inner])) {
				//handle collision directly
				//might want to handle collision by component instead
				auto outerMc = collisionDatas[outer].mc;
				outerMc->xpos = outerMc->oldXpos;
				outerMc->ypos = outerMc->oldYpos;

				auto innerMc = collisionDatas[inner].mc;
				innerMc->xpos = innerMc->oldXpos;
				innerMc->ypos = innerMc->oldYpos;
			};
		}
	}
}

unsigned int CollisionSystem::count() const {
	return collisionDatas.getSize();
}

const string CollisionSystem::getIdentifier() const {
	return "CollisionSystem";
}
