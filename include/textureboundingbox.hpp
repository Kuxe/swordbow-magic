#ifndef TEXTUREDBOUNDINGBOX_H
#define TEXTUREDBOUNDINGBOX_H

#include "boundingbox.hpp"

class Renderer;
class TextureBoundingBox : public BoundingBox {
private:
    const Renderer* renderer;
public:
    TextureBoundingBox(
        const ComponentManager* const componentManager,
        const Renderer* renderer
    );
    const Rect getBoundingBox(ID id) const;
    const Rect getOldBoundingBox(ID id) const;
    const string getSystemName() const;
};

#endif //TEXTUREDBOUNDINGBOX_H
