#include "sizeboundingbox.hpp"
#include "componentmanager.hpp"
#include "movecomponent.hpp"
#include "sizecomponent.hpp"

SizeBoundingBox::SizeBoundingBox(const ComponentManager* const componentManager):
    BoundingBox(componentManager) {}

const Rect SizeBoundingBox::getBoundingBox(ID id) const {
    try {
        const auto& mc = componentManager->moveComponents.at(id);
        const auto& sc = componentManager->sizeComponents.at(id);
        return Rect {
            mc.pos.x,
            mc.pos.y,
            static_cast<float>(sc.width - 1),
            static_cast<float>(sc.height - 1),
        };
    } catch(std::out_of_range oor) {
        std::ostringstream oss;
        oss << "Tried getting sizeboundingbox on id " << id << " but components or image were missing";
        Logger::log(oss, Log::ERROR);
        throw oor;
    }
}

const Rect SizeBoundingBox::getOldBoundingBox(ID id) const {
    try {
        const auto& mc = componentManager->moveComponents.at(id);
        const auto& sc = componentManager->sizeComponents.at(id);
        return Rect {
            floorf(mc.oldPos.x),
            floorf(mc.oldPos.y),
            static_cast<float>(sc.width - 1),
            static_cast<float>(sc.height - 1),
        };
    } catch(std::out_of_range oor) {
        std::ostringstream oss;
        oss << "Tried getting oldsizeboundingbox on id " << id << " but components or image were missing";
        Logger::log(oss, Log::ERROR);
        throw oor;
    }
}

const System::Identifier SizeBoundingBox::getSystemName() const {
    return System::HASHGRID_SIZE;
}
