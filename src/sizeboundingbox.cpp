#include "sizeboundingbox.h"
#include "componentmanager.h"
#include "movecomponent.h"
#include "sizecomponent.h"

SizeBoundingBox::SizeBoundingBox(const ComponentManager* const componentManager):
    BoundingBox(componentManager) {}

const SpatialIndexer::Rect SizeBoundingBox::getBoundingBox(ID id) const {
    const auto& mc = componentManager->moveComponents.at(id);
    const auto& sc = componentManager->sizeComponents.at(id);
    return SpatialIndexer::Rect {
        (unsigned int)mc.xpos,
        (unsigned int)mc.ypos,
        (unsigned int)(sc.width - 1),
        (unsigned int)(sc.height - 1),
    };
}

const SpatialIndexer::Rect SizeBoundingBox::getOldBoundingBox(ID id) const {
    const auto& mc = componentManager->moveComponents.at(id);
    const auto& sc = componentManager->sizeComponents.at(id);
    return SpatialIndexer::Rect {
        (unsigned int)mc.oldXpos,
        (unsigned int)mc.oldYpos,
        (unsigned int)(sc.width - 1),
        (unsigned int)(sc.height - 1),
    };
}

const string SizeBoundingBox::getSystemName() const {
    return "SizeHashGridSystem";
}
