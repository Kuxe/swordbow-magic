#ifndef TEXTUREDBOUNDINGBOX_H
#define TEXTUREDBOUNDINGBOX_H

#include "boundingbox.hpp"

class RenderSystem;
class TextureBoundingBox : public BoundingBox {
private:
    const RenderSystem* renderSystem;
public:
    TextureBoundingBox(
        const ComponentManager* const componentManager,
        const RenderSystem* rendersystem
    );
    const Rect getBoundingBox(ID id) const;
    const Rect getOldBoundingBox(ID id) const;
    const string getSystemName() const;
};

#endif //TEXTUREDBOUNDINGBOX_H
