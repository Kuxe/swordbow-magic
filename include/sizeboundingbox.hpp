#ifndef SIZEBOUNDINGBOX_H
#define SIZEBOUNDINGBOX_H

#include "boundingbox.hpp"

class SizeBoundingBox : public BoundingBox {
public:
    SizeBoundingBox(const ComponentManager* const componentManager);
    const Rect getBoundingBox(ID id) const;
    const Rect getOldBoundingBox(ID id) const;
    const System getSystemName() const;
};

#endif //SIZEBOUNDINGBOX_H
