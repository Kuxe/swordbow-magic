#ifndef SIZEBOUNDINGBOX_H
#define SIZEBOUNDINGBOX_H

#include "boundingbox.h"

class SizeBoundingBox : public BoundingBox {
public:
    SizeBoundingBox(const ComponentManager* const componentManager);
    const SpatialIndexer::Rect getBoundingBox(ID id) const;
    const SpatialIndexer::Rect getOldBoundingBox(ID id) const;
    const string getSystemName() const;
};

#endif //SIZEBOUNDINGBOX_H
