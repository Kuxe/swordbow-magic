#include "positionboundingbox.hpp"
#include "componentmanager.hpp"
#include "movecomponent.hpp"
#include <cmath>

/** For logging **/
#include "logger.hpp"
#include <ostream>

PositionBoundingBox::PositionBoundingBox(
    const ComponentManager* const componentManager
    ) :
    BoundingBox(componentManager) {}

const Rect PositionBoundingBox::getBoundingBox(ID id) const {
    try {
        auto& mc = componentManager->moveComponents.at(id);
        return Rect {
            floorf(mc.pos.x),
            floorf(mc.pos.y),
            1,
            1
        };
    } catch(std::out_of_range oor) {
        std::ostringstream oss;
        oss << "Tried getting oldboundingbox on id " << id << " but moveComponent is missing";
        Logger::log(oss, Log::ERROR);
        throw oor;
    }
}

const Rect PositionBoundingBox::getOldBoundingBox(ID id) const {
    try {
        auto& mc = componentManager->moveComponents.at(id);
        return Rect {
            floorf(mc.oldPos.x),
            floorf(mc.oldPos.y),
            100,
            100
        };
    } catch(std::out_of_range oor) {
        std::ostringstream oss;
        oss << "Tried getting oldboundingbox on id " << id << " but moveComponent is missing";
        Logger::log(oss, Log::ERROR);
        throw oor;
    }
}

const System::Identifier PositionBoundingBox::getSystemName() const {
    return System::HASHGRID_POSITION;
}
