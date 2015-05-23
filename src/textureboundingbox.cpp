#include "textureboundingbox.hpp"
#include "componentmanager.hpp"
#include "movecomponent.hpp"
#include "rendercomponent.hpp"

TextureBoundingBox::TextureBoundingBox(const ComponentManager* const componentManager):
    BoundingBox(componentManager) {}

const SpatialIndexer::Rect TextureBoundingBox::getBoundingBox(ID id) const {
    auto& mc = componentManager->moveComponents.at(id);
    auto& rc = componentManager->renderComponents.at(id);
    return SpatialIndexer::Rect {
        (unsigned int)(mc.pos.x + rc.xoffset),
        (unsigned int)(mc.pos.y + rc.yoffset),
        rc.textureData.width,
        rc.textureData.height,
    };
}

const SpatialIndexer::Rect TextureBoundingBox::getOldBoundingBox(ID id) const {
    auto& mc = componentManager->moveComponents.at(id);
    auto rc = componentManager->renderComponents.at(id);
    return SpatialIndexer::Rect {
        (unsigned int)(mc.oldPos.x + rc.xoffset),
        (unsigned int)(mc.oldPos.y + rc.yoffset),
        rc.textureData.width,
        rc.textureData.height,
    };
}

const string TextureBoundingBox::getSystemName() const {
    return "TextureHashGridSystem";
}
