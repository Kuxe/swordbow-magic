#include "textureboundingbox.hpp"
#include "componentmanager.hpp"
#include "movecomponent.hpp"
#include "rendercomponent.hpp"
#include "irenderer.hpp"
#include <cmath>

/** For logging **/
#include "logger.hpp"
#include <ostream>

TextureBoundingBox::TextureBoundingBox(const ComponentManager* const componentManager, IRenderer* const renderer)
    : BoundingBox(componentManager), renderer(renderer) { }

const Rect TextureBoundingBox::getBoundingBox(ID id) const {
    try {
        auto& mc = componentManager->moveComponents.at(id);
        auto& rc = componentManager->renderComponents.at(id);
        //auto& textureData = renderer->getTextureDatas().at(rc.image); //TODO: Use lowpoly3d
        return Rect {
            floorf(mc.pos.x + rc.offset.x),
            floorf(mc.pos.y + rc.offset.y),
            static_cast<float>(0.0/*textureData.dimension.x*/),
            static_cast<float>(0.0/*textureData.dimension.y*/),
        };
    } catch(std::out_of_range oor) {
        std::ostringstream oss;
        oss << "Tried getting textureboundingbox on id " << id << " but components or image were missing";
        Logger::log(oss, Logger::ERROR);
        throw oor;
    }
}

const Rect TextureBoundingBox::getOldBoundingBox(ID id) const {
    try {
        auto& mc = componentManager->moveComponents.at(id);
        auto& rc = componentManager->renderComponents.at(id);
        //auto& textureData = renderer->getTextureDatas().at(rc.image); //TODO: Use lowpoly3d
        return Rect {
            floorf(mc.oldPos.x + rc.offset.x),
            floorf(mc.oldPos.y + rc.offset.y),
            static_cast<float>(0.0/*textureData.dimension.x*/), //TODO: Use lowpoly3d
            static_cast<float>(0.0/*textureData.dimension.y*/), //TODO: Use lowpoly3d
        };
    } catch(std::out_of_range oor) {
        std::ostringstream oss;
        oss << "Tried getting oldtextureboundingbox on id " << id << " but components or image were missing";
        Logger::log(oss, Logger::ERROR);
        throw oor;
    }
}

const System::Identifier TextureBoundingBox::getSystemName() const {
    return System::HASHGRID_TEXTURE;
}
