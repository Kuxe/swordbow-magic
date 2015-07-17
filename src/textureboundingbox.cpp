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
    auto& textureData = renderer->getTextureDatas().at(rc.image);
    return Rect {
        floorf(mc.pos.x + rc.xoffset),
        floorf(mc.pos.y + rc.yoffset),
        static_cast<float>(textureData.width),
        static_cast<float>(textureData.height),
    };
}

const Rect TextureBoundingBox::getOldBoundingBox(ID id) const {
    auto& mc = componentManager->moveComponents.at(id);
    auto& rc = componentManager->renderComponents.at(id);
    auto& textureData = renderer->getTextureDatas().at(rc.image);
    return Rect {
        floorf(mc.oldPos.x + rc.xoffset),
        floorf(mc.oldPos.y + rc.yoffset),
        static_cast<float>(textureData.width),
        static_cast<float>(textureData.height),
    };
}

const System::Identifier TextureBoundingBox::getSystemName() const {
    return System::HASHGRID_TEXTURE;
}
