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
        (unsigned int)(mc->xpos + rc->xoffset),
        (unsigned int)(mc->ypos + rc->yoffset),
        rc->textureData.width,
        rc->textureData.height,
    };
}

const SpatialIndexer::Rect TextureBoundingBox::getOldBoundingBox(ID id) const {
    const MoveComponent* mc = componentManager->moveComponents.at(id);
    const RenderComponent* rc = componentManager->renderComponents.at(id);
    return SpatialIndexer::Rect {
        (unsigned int)(mc->oldXpos + rc->xoffset),
        (unsigned int)(mc->oldYpos + rc->yoffset),
        rc->textureData.width,
        rc->textureData.height,
    };
}

const string TextureBoundingBox::getSystemName() const {
    return "TextureHashGridSystem";
}
