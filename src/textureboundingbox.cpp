#include "textureboundingbox.hpp"
#include "componentmanager.hpp"
#include "movecomponent.hpp"
#include "rendercomponent.hpp"
#include "renderer.hpp"
#include <cmath>

TextureBoundingBox::TextureBoundingBox(
    const ComponentManager* const componentManager,
    const Renderer* renderer
    ) :
    BoundingBox(componentManager),
    renderer(renderer) {}

const Rect TextureBoundingBox::getBoundingBox(ID id) const {
    auto& mc = componentManager->moveComponents.at(id);
    auto& rc = componentManager->renderComponents.at(id);
    auto& textureData = renderer->getTextureDatas().at(rc.imagePath);
    return Rect {
        floor(mc.pos.x + rc.xoffset),
        floor(mc.pos.y + rc.yoffset),
        (float)textureData.width,
        (float)textureData.height,
    };
}

const Rect TextureBoundingBox::getOldBoundingBox(ID id) const {
    auto& mc = componentManager->moveComponents.at(id);
    auto& rc = componentManager->renderComponents.at(id);
    auto& textureData = renderer->getTextureDatas().at(rc.imagePath);
    return Rect {
        floor(mc.oldPos.x + rc.xoffset),
        floor(mc.oldPos.y + rc.yoffset),
        (float)textureData.width,
        (float)textureData.height,
    };
}

const System TextureBoundingBox::getSystemName() const {
    return System::HASHGRID_TEXTURE;
}
