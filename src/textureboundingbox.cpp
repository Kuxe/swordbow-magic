#include "textureboundingbox.h"
#include "componentmanager.h"
#include "movecomponent.h"
#include "rendercomponent.h"

TextureBoundingBox::TextureBoundingBox(const ComponentManager* const componentManager):
    BoundingBox(componentManager) {}

const SpatialIndexer::Rect TextureBoundingBox::getBoundingBox(ID id) const {
    const MoveComponent* mc = componentManager->moveComponents.at(id);
    const RenderComponent* rc = componentManager->renderComponents.at(id);
    return SpatialIndexer::Rect {
        mc->xpos + rc->xoffset,
        mc->ypos + rc->yoffset,
        rc->textureData.width - 1,
        rc->textureData.height - 1,
    };
}

const SpatialIndexer::Rect TextureBoundingBox::getOldBoundingBox(ID id) const {
    const MoveComponent* mc = componentManager->moveComponents.at(id);
    const RenderComponent* rc = componentManager->renderComponents.at(id);
    return SpatialIndexer::Rect {
        mc->oldXpos + rc->xoffset,
        mc->oldYpos + rc->yoffset,
        rc->textureData.width - 1,
        rc->textureData.height - 1,
    };
}

const string TextureBoundingBox::getSystemName() const {
    return "TextureHashGridSystem";
}
