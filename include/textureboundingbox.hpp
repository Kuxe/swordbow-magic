#ifndef TEXTUREDBOUNDINGBOX_H
#define TEXTUREDBOUNDINGBOX_H

#include "boundingbox.hpp"

class TextureBoundingBox : public BoundingBox {
public:
    TextureBoundingBox(const ComponentManager* const componentManager);
    const Rect getBoundingBox(ID id) const;
    const Rect getOldBoundingBox(ID id) const;
    const string getSystemName() const;
};

#endif //TEXTUREDBOUNDINGBOX_H
