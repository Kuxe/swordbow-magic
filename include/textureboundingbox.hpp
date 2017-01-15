#ifndef TEXTUREDBOUNDINGBOX_H
#define TEXTUREDBOUNDINGBOX_H

#include "boundingbox.hpp"

class IRenderer;
class TextureBoundingBox : public BoundingBox {
private:
    IRenderer* const renderer;
public:
    TextureBoundingBox(const ComponentManager* const componentManager, IRenderer* const renderer);
    const Rect getBoundingBox(ID id) const;
    const Rect getOldBoundingBox(ID id) const;
    const System::Identifier getSystemName() const;
};

#endif //TEXTUREDBOUNDINGBOX_H
