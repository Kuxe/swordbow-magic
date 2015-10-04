#include "hashgridsystem.hpp"
#include "boundingbox.hpp"

/** For logging **/
#include "logger.hpp"

HashGridSystem::HashGridSystem(
	BoundingBox* boundingBox,
	const unsigned int worldWidth,
	const unsigned int worldHeight,
	const unsigned int side) :
		width(worldWidth / side + (worldWidth % side != 0)),
		height(worldHeight / side + (worldHeight % side != 0)),
		cellsCapacity(width * height),
		cells(new std::unordered_set<ID>[cellsCapacity]),
		side(side),
		boundingBox(boundingBox) { }

HashGridSystem::~HashGridSystem() {
	delete[] cells;
}

void HashGridSystem::add(const ID id) {
	ids.insert(id);
	addToCells(id);
}

void HashGridSystem::remove(const ID id) {
	ids.erase(id);
	removeFromCells(id);
}

void HashGridSystem::addToCells(const ID id) {
	const auto bb = boundingBox->getBoundingBox(id);

	std::ostringstream oss;
	oss << "Adding id " << id << " to hashgrid cells";
	Logger::log(oss, Log::INFO);

	//Place ID in all cells which partially or completely contains the ID
	for(unsigned int y = bb.y/side; y < (bb.y + bb.h)/side; y++) {
		for(unsigned int x = bb.x/side; x < (bb.x + bb.w)/side; x++) {
			if(y*width + x < cellsCapacity) {
				cells[y*width + x].insert(id);
			} else {
				Logger::log("Tried to index out of HashGrid cellsCapacity", Log::WARNING);
			}
		}
	}
}

void HashGridSystem::removeFromCells(const ID id) {
	try {
		const auto bb = boundingBox->getBoundingBox(id);

		//Remove ID in all cells which previously contained this ID
		for(unsigned int y = bb.y/side; y < (bb.y + bb.h)/side; y++) {
			for(unsigned int x = bb.x/side; x < (bb.x + bb.w)/side; x++) {
				if(cells[y*width + x].erase(id) == 0) {
					Logger::log(
						std::to_string(getIdentifier()) +
						std::string(
							" couldn't erase " +
							std::to_string(id) +
							" from grid because it didn't exist"
						), Log::WARNING
					);
				};
			}
		}
	} catch (std::out_of_range oor) {
		std::ostringstream oss;
		oss << "Couldn't remove id " << id << " from hashgrid-cells using bounding box";
		oss << " because the bounding box couldn't be retrieved";
		Logger::log(oss, Log::ERROR);
		throw oor;
	}
}

void HashGridSystem::removeFromCellsOldBoundingBox(const ID id) {
	try {
		const auto bb = boundingBox->getOldBoundingBox(id);

		//Remove ID in all cells which previously contained this ID
		for(unsigned int y = bb.y/side; y < (bb.y + bb.h)/side; y++) {
			for(unsigned int x = bb.x/side; x < (bb.x + bb.w)/side; x++) {
				if(cells[y*width + x].erase(id) == 0) {
					Logger::log(
						std::to_string(getIdentifier()) +
						std::string(
							" couldn't erase " +
							std::to_string(id) +
							" (old boundingbox) from grid because it didn't exist"
						), Log::WARNING
					);
				};
			}
		}
	} catch (std::out_of_range oor) {
		std::ostringstream oss;
		oss << "Couldn't remove id " << id << " from hashgrid-cells using old bounding box";
		oss << " because the old bounding box couldn't be retrieved";
		Logger::log(oss, Log::ERROR);
		throw oor;
	}
}

//Returns all entities that are fully or partly contained by id's bounding box
std::unordered_set<ID> HashGridSystem::overlaps(const ID id) const {
	auto overlappingIds = query(boundingBox->getBoundingBox(id));
	overlappingIds.erase(id);
	return overlappingIds;
}

//Return all entities that are fully or partly contained by a queryArea
std::unordered_set<ID> HashGridSystem::query(const Rect& queryArea) const {
	std::unordered_set<ID> queryIds;

	//Loop through all cells in which this ID is partly or fully contained
	for(unsigned int y = queryArea.y/side; y < (queryArea.y + queryArea.h)/side; y++) {
		for(unsigned int x = queryArea.x/side; x < (queryArea.x + queryArea.w)/side; x++) {

			//For all ids in the same cells as this one..
			for(auto otherId : cells[y*width + x]) {
				auto bb = boundingBox->getBoundingBox(otherId);
				if(Rect::intersect(queryArea, bb)) {
					queryIds.insert(otherId);
				}
			}
		}
	}
	return queryIds;
}

Rect HashGridSystem::getBoundingBox(ID id) const {
	try {
		return boundingBox->getBoundingBox(id);
	} catch(std::out_of_range oor) {
		std::ostringstream oss;
		oss << "HashGridSystem couldn't get the bounding box of id " << id;
		Logger::log(oss, Log::ERROR);
		throw oor;
	}
}

bool HashGridSystem::activateId(const ID id) {
	//if the id is a member of hashgrid...
	if(ids.find(id) != ids.end()) {
		//then it is legit to make it an active id
		activeIds.push(id);
		return true;
	}
	return false;
}

void HashGridSystem::update() {
	while(!activeIds.empty()) {
		auto id = activeIds.front(); activeIds.pop();

		//search cells bounded by old bounding box for id
		//and remove id from those cells
		try {
			removeFromCellsOldBoundingBox(id);
		} catch (std::out_of_range oor) {
			std::ostringstream oss;
			oss << "Call to removeFromCellsOldBoundingBox failed for id " << id;
			Logger::log(oss, Log::ERROR);
			throw oor;
		}

		//add id to the cells bounded by current bounding box for id
		addToCells(id);
	}
}

unsigned int HashGridSystem::count() const {
	return ids.size();
}
const System::Identifier HashGridSystem::getIdentifier() const {
	return boundingBox->getSystemName();
}
