#include "sizeboundingbox.hpp"
#include "componentmanager.hpp"
#include "movecomponent.hpp"
#include "sizecomponent.hpp"

SizeBoundingBox::SizeBoundingBox(const ComponentManager* const componentManager):
    BoundingBox(componentManager) {}

const Rect SizeBoundingBox::getBoundingBox(ID id) const {
    const auto& mc = componentManager->moveComponents.at(id);
    const auto& sc = componentManager->sizeComponents.at(id);
    return Rect {
        (float)mc.pos.x,
        (float)mc.pos.y,
        (float)(sc.width - 1),
        (float)(sc.height - 1),
    };
}

const Rect SizeBoundingBox::getOldBoundingBox(ID id) const {
    const auto& mc = componentManager->moveComponents.at(id);
    const auto& sc = componentManager->sizeComponents.at(id);
    return Rect {
        (float)mc.oldPos.x,
        (float)mc.oldPos.y,
        (float)(sc.width - 1),
        (float)(sc.height - 1),
    };
}

const System::Identifier SizeBoundingBox::getSystemName() const {
    return System::HASHGRID_SIZE;
}
