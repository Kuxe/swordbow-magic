#ifndef TEXTUREDBOUNDINGBOX_H
#define TEXTUREDBOUNDINGBOX_H

#include "boundingbox.h"

class TextureBoundingBox : public BoundingBox {
public:
    TextureBoundingBox(const ComponentManager* const componentManager);
    const SpatialIndexer::Rect getBoundingBox(ID id) const;
    const SpatialIndexer::Rect getOldBoundingBox(ID id) const;
    const string getSystemName() const;
};

#endif //TEXTUREDBOUNDINGBOX_H
