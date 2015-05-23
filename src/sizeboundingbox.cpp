#include "sizeboundingbox.hpp"
#include "componentmanager.hpp"
#include "movecomponent.hpp"
#include "sizecomponent.hpp"

SizeBoundingBox::SizeBoundingBox(const ComponentManager* const componentManager):
    BoundingBox(componentManager) {}

const SpatialIndexer::Rect SizeBoundingBox::getBoundingBox(ID id) const {
    const auto& mc = componentManager->moveComponents.at(id);
    const auto& sc = componentManager->sizeComponents.at(id);
    return SpatialIndexer::Rect {
        (unsigned int)mc.pos.x,
        (unsigned int)mc.pos.y,
        (unsigned int)(sc.width - 1),
        (unsigned int)(sc.height - 1),
    };
}

const SpatialIndexer::Rect SizeBoundingBox::getOldBoundingBox(ID id) const {
    const auto& mc = componentManager->moveComponents.at(id);
    const auto& sc = componentManager->sizeComponents.at(id);
    return SpatialIndexer::Rect {
        (unsigned int)mc.oldPos.x,
        (unsigned int)mc.oldPos.y,
        (unsigned int)(sc.width - 1),
        (unsigned int)(sc.height - 1),
    };
}

const string SizeBoundingBox::getSystemName() const {
    return "SizeHashGridSystem";
}
